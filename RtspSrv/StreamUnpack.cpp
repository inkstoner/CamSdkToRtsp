//
// Created by ubosm on 2021/7/29.
//

#include <cstring>
#include <unistd.h>
#include <rtsp_demo.h>
#include "StreamUnpack.h"

void StreamUnpack::Init(rtsp_codec_id _codeId) {
    codeId = _codeId;
    if (m_buffer == nullptr)
        m_buffer = new char[m_buffer_size];
}

void StreamUnpack::Start(Stream_CallBack _streamCallBack) {
    streamCallBack = _streamCallBack;

    pthread_create(&m_read_thread, 0, start_thread, this);
}

void StreamUnpack::Stop() {
     m_exit = true;
}

void StreamUnpack::SaveStream(unsigned char *_buffer, int _buffer_size) {
    if (m_buffer_size - m_buffer_offset < _buffer_size) {
        printf("save_ps_stream drop frame. No enough space. _buffer_size(%d), m_ps_buffer_offset(%d), left space(%d)\n",
               _buffer_size, m_buffer_offset, m_buffer_size - m_buffer_offset);
        return;
    }

    m_read_lock.lock();
    memcpy(m_buffer + m_buffer_offset, _buffer, _buffer_size);
    m_buffer_offset += _buffer_size;
    m_read_lock.unlock();
}

void *StreamUnpack::start_thread(void *pArgs) {
    StreamUnpack *ptr = (StreamUnpack *) pArgs;
    ptr->read_stream_thread();
    return nullptr;
}

void StreamUnpack::read_stream_thread() {

    bool bFindStart, bFindEnd;
    int i = 0;
    int s32UsedBytes = 0, s32ReadLen = 0;

    while (!m_exit) {
        if (m_buffer_offset < 10 * 1024) {
            usleep(40 * 1000);
            continue;
        }

        m_read_lock.lock();

        bFindStart = false;
        bFindEnd = false;
        char *pu8Buf = m_buffer;
        s32ReadLen = m_buffer_offset;

        if (codeId == rtsp_codec_id::RTSP_CODEC_ID_VIDEO_H264) {

            for (i = 0; i < s32ReadLen - 8; i++) {
                int tmp = pu8Buf[i + 3] & 0x1F;
                if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                    (
                            ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
                            (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)
                    )
                        ) {
                    bFindStart = true;
                    i += 8;
                    break;
                }
            }

            for (; i < s32ReadLen - 8; i++) {
                int tmp = pu8Buf[i + 3] & 0x1F;
                if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1 &&
                    (
                            tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
                            ((tmp == 5 || tmp == 1) && ((pu8Buf[i + 4] & 0x80) == 0x80)) ||
                            (tmp == 20 && (pu8Buf[i + 7] & 0x80) == 0x80)
                    )
                        ) {
                    bFindEnd = true;
                    break;
                }
            }

            if (i > 0) s32ReadLen = i;
            if (bFindStart == false) {
                printf("Get Upack:  can not find start code!s32ReadLen %d, s32UsedBytes %d. \n",
                       s32ReadLen, s32UsedBytes);
            } else if (bFindEnd == false) {
                s32ReadLen = i + 8;
            }

        } else if (codeId == rtsp_codec_id::RTSP_CODEC_ID_VIDEO_H265) {
            bool bNewPic = false;

            for (i = 0; i < s32ReadLen - 6; i++) {
                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                           && (((pu8Buf[i + 3] & 0x7D) >= 0x0 && (pu8Buf[i + 3] & 0x7D) <= 0x2A) ||
                               (pu8Buf[i + 3] & 0x1F) == 0x1)
                           && ((pu8Buf[i + 5] & 0x80) == 0x80));//first slice segment

                if (bNewPic) {
                    bFindStart = true;
                    i += 4;
                    break;
                }
            }

            for (; i < s32ReadLen - 6; i++) {
                bNewPic = (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                           && (((pu8Buf[i + 3] & 0x7D) >= 0x0 && (pu8Buf[i + 3] & 0x7D) <= 0x2A) ||
                               (pu8Buf[i + 3] & 0x1F) == 0x1)
                           && ((pu8Buf[i + 5] & 0x80) == 0x80));//first slice segment

                if (pu8Buf[i] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1
                    && (((pu8Buf[i + 3] & 0x7D) == 0x40) || ((pu8Buf[i + 3] & 0x7D) == 0x42) ||
                        ((pu8Buf[i + 3] & 0x7D) == 0x44) || bNewPic)
                        ) {
                    bFindEnd = true;
                    break;
                }
            }

            s32ReadLen = i;

            if (bFindStart == false) {
                printf("hevc can not find start code! %d s32ReadLen 0x%x +++++++++++++\n", codeId, s32ReadLen);
            } else if (bFindEnd == false) {
                s32ReadLen = i + 6;
            }
        }

        if (streamCallBack != NULL) {
            streamCallBack((unsigned char *) pu8Buf, s32ReadLen);
        }

        memmove(m_buffer, m_buffer + s32ReadLen, m_buffer_offset - s32ReadLen);
        m_buffer_offset -= s32ReadLen;
        m_read_lock.unlock();
    }
}

StreamUnpack::~StreamUnpack() {
    delete[] m_buffer;
}
