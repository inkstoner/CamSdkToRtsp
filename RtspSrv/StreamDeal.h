//
// Created by ubosm on 2021/7/28.
//

#ifndef SDKTORTSP_STREAMDEAL_H
#define SDKTORTSP_STREAMDEAL_H

#include <rtsp_demo.h>
#include "../Common/PsHelper.h"

class StreamDeal {
public:
    StreamDeal(int port,int codeId);
    ~StreamDeal();
    void save_H264_stream(unsigned char *_buffer, int _buffer_size);
    void save_unpacke_stream(unsigned char *_buffer, int _buffer_size);

    PsHelper* getPsHelper()
    {
        return psHelper;
    }

private:
    PsHelper* psHelper = NULL;
    StreamUnpack* streamUnpack = NULL;

    long long pts = 0;

    rtsp_session_handle session = nullptr;
//    NngPub* nngPub = NULL;
};


#endif //SDKTORTSP_STREAMDEAL_H
