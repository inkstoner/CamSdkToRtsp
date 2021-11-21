//
// Created by ubosm on 2021/11/12.
//

#ifndef SDKTORTSP_PUSHDEAL_H
#define SDKTORTSP_PUSHDEAL_H

//推流器
#include "../Common/PsHelper.h"
#include "FFmpegPusher.h"

class PushDeal {
public:
    PushDeal(int port);

    ~PushDeal();

    void save_H264_stream(unsigned char *_buffer, int _buffer_size);

    PsHelper* getPsHelper()
    {
        return psHelper;
    }

private:
    PsHelper* psHelper = NULL;
    FFmpegPusher* fFmpegPusher = NULL;
};


#endif //SDKTORTSP_PUSHDEAL_H
