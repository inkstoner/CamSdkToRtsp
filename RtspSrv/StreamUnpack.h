//
// Created by ubosm on 2021/7/29.
//

#ifndef SDKTORTSP_STREAMUNPACK_H
#define SDKTORTSP_STREAMUNPACK_H

#include <pthread.h>
#include <mutex>
#include "../CameraSDK/ICameraDefine.h"
#include "rtsp_demo.h"

class StreamUnpack {
public:
    ~StreamUnpack();
    void Init(rtsp_codec_id _codeId = rtsp_codec_id::RTSP_CODEC_ID_VIDEO_H264);
    void Start(Stream_CallBack _streamCallBack = NULL);
    void Stop();

    void SaveStream(unsigned char *_buffer, int _buffer_size);
private:
    static void *start_thread(void *pArgs);

    void read_stream_thread();

private:
    pthread_t m_read_thread;
    bool m_exit = false;
    mutable std::mutex m_read_lock;

    char *m_buffer = nullptr;
    static constexpr int m_buffer_size = 10 * 1024 * 1024;

    int m_buffer_offset = 0;
    Stream_CallBack streamCallBack = NULL;

    rtsp_codec_id codeId = rtsp_codec_id::RTSP_CODEC_ID_VIDEO_H264;
};


#endif //SDKTORTSP_STREAMUNPACK_H
