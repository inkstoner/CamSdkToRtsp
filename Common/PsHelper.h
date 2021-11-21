//
// Created by ubosm on 2021/7/28.
//

#ifndef SDKTORTSP_PSHELPER_H
#define SDKTORTSP_PSHELPER_H

#include <pthread.h>
#include <mutex>

#include "../Utils/litets/litets.h"
#include "../CameraSDK/ICameraDefine.h"
#include "../RtspSrv/StreamUnpack.h"

class PsHelper {
public:
    ~PsHelper();
    void Init();

    void Start(Stream_CallBack _streamCallBack = NULL);

    void Stop();

    void save_ps_stream(unsigned char *_buffer, int _buffer_size);

private:
    static void *start_thread(void *pArgs);

    void read_ps_stream_thread();

private:
    pthread_t m_demux_ps_thread;
    bool m_exit = false;
    mutable std::mutex m_ps_buffer_lock;

    TDemux m_demux;
    char *m_ps_buffer = nullptr;
    static constexpr int m_ps_buffer_size = 10 * 1024 * 1024;
    int m_ps_buffer_offset = 0;

    Stream_CallBack streamCallBack = NULL;
};


#endif //SDKTORTSP_PSHELPER_H
