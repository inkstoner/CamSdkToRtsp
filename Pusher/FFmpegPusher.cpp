//
// Created by ubosm on 2021/11/12.
//

#include <unistd.h>
#include <thread>
#include "FFmpegPusher.h"
#include "../Common/Config.hpp"

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#ifdef __cplusplus
};
#endif
#endif

#define PUSHBUFFSIZE 8192

static bool isInitFFmpeg = false;

FFmpegPusher::FFmpegPusher() {
    ringBufferS = new ring_buffer_s(32768 * 10);
}

void FFmpegPusher::SaveStream(unsigned char *_buffer, int _buffer_size) {
    //std::cout << "--pusher recive h264 " << _buffer_size << " ringBufferS size " << ringBufferS->size() << std::endl;
    ringBufferS->write(_buffer, _buffer_size);
}

FFmpegPusher::~FFmpegPusher() {
    delete ringBufferS;
}

void FFmpegPusher::Start(int port) {
    m_port = port;
    pthread_create(&m_push_thread, 0, start_thread, this);
}

void FFmpegPusher::Stop() {
    m_exit = true;
}

void *FFmpegPusher::start_thread(void *pArgs) {
    FFmpegPusher *ptr = (FFmpegPusher *) pArgs;
    ptr->push_stream_thread();
    return nullptr;
}

void FFmpegPusher::push_stream_thread() {
    if (!isInitFFmpeg) {
        av_register_all();
        //Network
        avformat_network_init();
        isInitFFmpeg = true;
    }

    while (!m_exit) {
        while (ringBufferS->size() < PUSHBUFFSIZE * 4 ) {
            std::cout << "---- ringBufferS not end " << ringBufferS->size() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }

        std::string outAddress =
                "rtmp://" + Config::getInstance().getPushServer() + "/live/push" + std::to_string(m_port);

        AVOutputFormat *ofmt = NULL;
        //Input AVFormatContext and Output AVFormatContext
        AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
        AVInputFormat *piFmt = NULL;
        AVPacket pkt;

        int ret, i;
        int videoindex = -1;
        int frame_index = 0;
        int64_t start_time = 0;

        /*内存*/
        ifmt_ctx = avformat_alloc_context();
        unsigned char *inbuffer = NULL;
        inbuffer = (unsigned char *) av_malloc(PUSHBUFFSIZE);
        AVIOContext *avio_in = NULL;
        avio_in = avio_alloc_context(inbuffer, PUSHBUFFSIZE, 0, this, read_buffer, NULL, NULL);
        if (avio_in == NULL)
            goto end;

        //探测从内存中获取到的媒体流的格式
        if (av_probe_input_buffer(avio_in, &piFmt, "", NULL, 0, 0) < 0) {
            printf("probe format failed\n");
            goto end;
        } else {
            printf("format:%s[%s]\n", piFmt->name, piFmt->long_name);
        }

        ifmt_ctx->pb = avio_in;

        //Input
        if ((ret = avformat_open_input(&ifmt_ctx, "", piFmt, NULL)) < 0) {
            printf("Could not open input file.");
            goto end;
        }
        if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
            printf("Failed to retrieve input stream information");
            goto end;
        }

        for (i = 0; i < ifmt_ctx->nb_streams; i++) {
            if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoindex = i;
                break;
            }
        }

        //Output

        avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", outAddress.c_str()); //RTMP
        //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP

        if (!ofmt_ctx) {
            printf("Could not create output context\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        ofmt = ofmt_ctx->oformat;
        for (i = 0; i < ifmt_ctx->nb_streams; i++) {
            //Create output AVStream according to input AVStream
            AVStream *in_stream = ifmt_ctx->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
            if (!out_stream) {
                printf("Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            //Copy the settings of AVCodecContext
            ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
            if (ret < 0) {
                printf("Failed to copy context from input to output stream codec context\n");
                goto end;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
        //Dump Format------------------
        av_dump_format(ofmt_ctx, 0, outAddress.c_str(), 1);
        //Open output URL
        if (!(ofmt->flags & AVFMT_NOFILE)) {
            ret = avio_open(&ofmt_ctx->pb, outAddress.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0) {
                printf("Could not open output URL '%s'", outAddress.c_str());
                goto end;
            }
        }
        //Write file header
        ret = avformat_write_header(ofmt_ctx, NULL);
        if (ret < 0) {
            printf("Error occurred when opening output URL\n");
            goto end;
        }

        start_time = av_gettime();
        while (!m_exit) {
            AVStream *in_stream, *out_stream;
            //Get an AVPacket
            ret = av_read_frame(ifmt_ctx, &pkt);
            if (ret < 0) {
                std::cout << "av_read_frame error ring size :" << ringBufferS->size() << std::endl;
                //没数据了 等几毫秒
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
//                break;
            }

            //FIX：No PTS (Example: Raw H.264)
            //Simple Write PTS
            if (pkt.pts == AV_NOPTS_VALUE) {
                //Write PTS
                AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
                //Duration between 2 frames (us)
                int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
                //Parameters
                pkt.pts = (double) (frame_index * calc_duration) / (double) (av_q2d(time_base1) * AV_TIME_BASE);
                pkt.dts = pkt.pts;
                pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            }
            //Important:Delay
            if (pkt.stream_index == videoindex) {
                AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
                AVRational time_base_q = {1, AV_TIME_BASE};
                int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
                int64_t now_time = av_gettime() - start_time;
                if (pts_time > now_time)
                    av_usleep(pts_time - now_time);

            }

            in_stream = ifmt_ctx->streams[pkt.stream_index];
            out_stream = ofmt_ctx->streams[pkt.stream_index];
            /* copy packet */
            //Convert PTS/DTS
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
            //Print to Screen
            if (pkt.stream_index == videoindex) {
                if (frame_index % 1000 == 0)
                    printf("Port %d Send %8d video frames \n", m_port, frame_index);
                frame_index++;
            }
            //ret = av_write_frame(ofmt_ctx, &pkt);
            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

            if (ret < 0) {
                printf("Error muxing packet\n");
                break;
            }

            av_free_packet(&pkt);

        }
        //Write file trailer
        av_write_trailer(ofmt_ctx);
        end:
        avformat_close_input(&ifmt_ctx);
        /* close output */
        if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
            avio_close(ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
//        av_free(inbuffer);
        if (ret < 0 && ret != AVERROR_EOF) {
            printf("Error occurred.\n");
            continue;
//            return;
        }
    }
}

int FFmpegPusher::read_buffer(void *opaque, uint8_t *buf, int buf_size) {
    FFmpegPusher *ptr = (FFmpegPusher *) opaque;
    int true_size = ptr->readBuffer(buf, buf_size);
    return true_size;
}

int FFmpegPusher::readBuffer(uint8_t *buf, int buf_size) {
    int trueReadlen = ringBufferS->read(buf, buf_size);
    ringReadCount++;
    if (ringReadCount % 50 == 0) {
        printf("port %d readbuff %d cache %d \n", m_port, trueReadlen, ringBufferS->size());
    }

    return trueReadlen;
}
