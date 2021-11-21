//
// Created by ubosm on 2021/11/12.
//

#include "PushDeal.h"

PushDeal::PushDeal(int port) {
    psHelper = new PsHelper;
    psHelper->Init();
    Stream_CallBack streamCallBack = std::bind(&PushDeal::save_H264_stream, this, std::placeholders::_1,
                                               std::placeholders::_2);
    psHelper->Start(streamCallBack);

    fFmpegPusher = new FFmpegPusher;
    fFmpegPusher->Start(port);
}

void PushDeal::save_H264_stream(unsigned char *_buffer, int _buffer_size) {
    fFmpegPusher->SaveStream(_buffer,_buffer_size);
}

PushDeal::~PushDeal() {
    psHelper->Stop();
    delete psHelper;

    fFmpegPusher->Stop();
    delete fFmpegPusher;

}
