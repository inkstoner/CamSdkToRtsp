//
// Created by ubosm on 2021/7/28.
//

#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

#include "PsHelper.h"

void PsHelper::Start(Stream_CallBack _streamCallBack) {
    streamCallBack = _streamCallBack;

    m_ps_buffer_offset = 0;
    memset(&m_demux, 0, sizeof(m_demux));
    pthread_create(&m_demux_ps_thread, 0, start_thread, this);
}

void PsHelper::Stop() {
    m_exit = true;
}

void *PsHelper::start_thread(void *pArgs) {
    PsHelper *ptr = (PsHelper *) pArgs;
    ptr->read_ps_stream_thread();
    return nullptr;
}

void PsHelper::read_ps_stream_thread() {

    while (!m_exit) {
        if (m_ps_buffer_offset < 10 * 1024) {
            usleep(40 * 1000);
            continue;
        }

        m_ps_buffer_lock.lock();
        int parsed = 0;
        while (true)
        {
            int len = lts_ps_demux(&m_demux, (uint8_t *)m_ps_buffer + parsed, m_ps_buffer_offset - parsed);
            if (len > 0)
            {
                if (m_demux.is_pes)
                {
                    if(streamCallBack!= NULL)
                    {
                        streamCallBack(m_demux.es_ptr, m_demux.es_len);
                    }
                }
            }
            else
                break;

            parsed += len;
        }
        memmove(m_ps_buffer, m_ps_buffer + parsed, m_ps_buffer_offset - parsed);
        m_ps_buffer_offset -= parsed;
        m_ps_buffer_lock.unlock();
    }
}

void PsHelper::Init() {
    if (m_ps_buffer == nullptr)
        m_ps_buffer = new char[m_ps_buffer_size];
    m_ps_buffer_offset = 0;
}

void PsHelper::save_ps_stream(unsigned char *_buffer, int _buffer_size) {
    if (m_ps_buffer_size - m_ps_buffer_offset < _buffer_size)
    {
        std::cout << "save_ps_stream drop frame. No enough space"  << std::endl;
        printf("save_ps_stream drop frame. No enough space. _buffer_size(%d), m_ps_buffer_offset(%d), left space(%d)\n",
                  _buffer_size, m_ps_buffer_offset, m_ps_buffer_size - m_ps_buffer_offset);
        return;
    }

    m_ps_buffer_lock.lock();
//    std::cout << "sdk recive data " << _buffer_size << std::endl;
    memcpy(m_ps_buffer + m_ps_buffer_offset, _buffer, _buffer_size);
    m_ps_buffer_offset += _buffer_size;
    m_ps_buffer_lock.unlock();
}

PsHelper::~PsHelper() {
    delete[] m_ps_buffer;
}
