/*********************************************************************************
*FileName:				ICameraSDK.h
*
*Author:				simple
*
*Date:					2021-03-09
*
*Description:			摄像头SDK接口;
**********************************************************************************/

#ifndef ICameraSDK_H
#define ICameraSDK_H

#include <string>
#include <vector>
#include "ICameraDefine.h"

class ICameraSDK{
public:
    //登录;
    virtual bool login(std::string ip,std::string username,std::string password, int port)=0;
    //登出;
    virtual bool loginOut()=0;
    //抓图;
    virtual bool capturePicture(int channelIndex,std::string& path)=0;
    virtual bool captureStream(int channelIndex,Stream_CallBack _cb, void* _user_data, long& _capture_handle, int streamType) = 0;
    //获取可用通道;
    virtual std::vector<SRCam::stChannelInfo> aliveChannel()=0;
    //云台控制
    virtual bool ptzControl(int channelIndex, SRCam::PTZType ptzType, SRCam::stPTZControlInfo controlInfo)=0;
    //预置点操作
    virtual bool presetControl(int channelIndex, SRCam::PTZType ptzType, int presetIndex)=0;
    //错误码;
    virtual int errorCode(){ return m_errorCode;}
    //获取摄像头信息;
    SRCam::stCameraInfo getCameraInfo(){ return  m_cameraInfo;}
    //开启热成像温度监控
    virtual bool startThermometry(int cameraChannel,Temperature_CallBack temperatureCallBack){return false;}
    //关闭热成像温度监控
    virtual bool stopThermometry(){return false;}
    //获取当前摄像头PTZ坐标信息;
    virtual bool getCurrentPtzInfo(SRCam::stPTZInfo& ptzInfo, int channelIndex){ return false;}
    //获取当前摄像头某预置点PTZ坐标信息;
    virtual bool getPresetPtzInfo(SRCam::stPTZInfo& ptzInfo, int channelIndex, int presetIndex){ return false;}

protected:
    //摄像头信息;
    SRCam::stCameraInfo m_cameraInfo;
    //错误码;
    int m_errorCode;
};
#endif
