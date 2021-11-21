//
// Created by ubosm on 2021/11/12.
//

#ifndef SDKTORTSP_FFMPEGPUSHER_H
#define SDKTORTSP_FFMPEGPUSHER_H


#include "../Utils/ring_buffer_s.h"


class FFmpegPusher {
public:
    FFmpegPusher();
    ~FFmpegPusher();

    void Start(int port);
    void Stop();

    void SaveStream(unsigned char *_buffer, int _buffer_size);

private:
    static void *start_thread(void *pArgs);
    static int read_buffer(void *opaque, uint8_t *buf, int buf_size);
    void push_stream_thread();
    int readBuffer(uint8_t *buf, int buf_size);
private:
    pthread_t m_push_thread;
    bool m_exit = false;

    ring_buffer_s* ringBufferS = NULL;

    int m_port;
    long ringReadCount = 0;
};


#endif //SDKTORTSP_FFMPEGPUSHER_H
