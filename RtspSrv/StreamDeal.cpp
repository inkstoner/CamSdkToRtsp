//
// Created by ubosm on 2021/7/28.
//

#include "StreamDeal.h"
#include <string>
#include <cstring>
#include <mutex>

rtsp_demo_handle g_rtsplive = NULL;
std::mutex g_lock;

StreamDeal::StreamDeal(int port, int codeId) {
    if (g_rtsplive == NULL) {
        g_rtsplive = create_rtsp_demo(1554);
    }

    std::string suffix = "/live" + std::to_string(port);
    session = create_rtsp_session2(g_rtsplive, suffix.c_str(),static_cast<rtsp_codec_id> (codeId));

    psHelper = new PsHelper;
    psHelper->Init();
    Stream_CallBack streamCallBack = std::bind(&StreamDeal::save_H264_stream, this, std::placeholders::_1,
                                               std::placeholders::_2);
    psHelper->Start(streamCallBack);

    streamUnpack = new StreamUnpack();
    streamUnpack->Init(static_cast<rtsp_codec_id> (codeId));
    Stream_CallBack unpackstreamCallBack = std::bind(&StreamDeal::save_unpacke_stream, this, std::placeholders::_1,
                                                     std::placeholders::_2);
    streamUnpack->Start(unpackstreamCallBack);
}

void StreamDeal::save_H264_stream(unsigned char *_buffer, int _buffer_size) {
    streamUnpack->SaveStream(_buffer, _buffer_size);
}

void StreamDeal::save_unpacke_stream(unsigned char *_buffer, int _buffer_size) {
    if (g_rtsplive) {
        std::vector<unsigned char> data(_buffer_size);
        memcpy(&data[0], _buffer, _buffer_size);
        pts += 40;
        g_lock.lock();
        rtsp_sever_tx_video(g_rtsplive, session, &data[0], _buffer_size, pts);
        g_lock.unlock();
    }
}

StreamDeal::~StreamDeal() {
    psHelper->Stop();
    delete psHelper;
    streamUnpack->Stop();
    delete streamUnpack;
}
