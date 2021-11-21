/*********************************************************************************
*FileName:				CameraHK.h
*
*Author:				simple
*
*Date:					2021-03-09
*
*Description:			海康摄像头;
**********************************************************************************/
#ifndef CameraHK_H
#define CameraHK_H

#include "ICameraSDK.h"
#include "HCNetSDK.h"
class CameraHK:public ICameraSDK
{
public:
    //初始化摄像头;
    static void init();
    //清理摄像头;
    static void clean();
    //登录;
    virtual bool login(std::string ip,std::string username,std::string password, int port);
    //登出;
    virtual bool loginOut();
    //抓图;
    virtual bool capturePicture(int channelIndex,std::string& path);
    virtual bool captureStream(int channelIndex,Stream_CallBack _cb, void* _user_data, long& _capture_handle, int streamType);
    //获取可用通道;
    virtual std::vector<SRCam::stChannelInfo> aliveChannel();
    //云台控制
    virtual bool ptzControl(int channelIndex, SRCam::PTZType ptzType, SRCam::stPTZControlInfo controlInfo);
    //预置点操作
    virtual bool presetControl(int channelIndex, SRCam::PTZType ptzType, int presetIndex);
    //开启热成像温度监控
    virtual bool startThermometry(int cameraChannel,Temperature_CallBack temperatureCallBack);
    //关闭热成像温度监控
    virtual bool stopThermometry();

public:
    void setTemperatureData(std::string fMaxTemperature,std::string fMinTemperature,std::string fAverageTemperature,std::string fTemperatureDiff);

protected:
    std::string channelName(int channelIndex);

private:
    NET_DVR_DEVICEINFO_V30 m_deviceInfo;
    //登录ID
    long m_loginID=-1;
    //热成像温度相关
    Temperature_CallBack m_temperatureCallBack=NULL;
    long m_lRealTimeInfoHandle=-1;

    static void CALLBACK real_data_cb(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser);
};

#endif
