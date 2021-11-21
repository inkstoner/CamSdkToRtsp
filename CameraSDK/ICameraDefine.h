/*********************************************************************************
*FileName:				ICameraDefine.h
*
*Author:				simple
*
*Date:					2021-03-09
*
*Description:			摄像头SDK接口相关定义;
**********************************************************************************/

#ifndef ICameraDefine_H
#define ICameraDefine_H
#include <string>
#include <functional>
#include <vector>

//热成像温度回调函数
typedef  std::function<void(std::string,std::string,std::string,std::string)> Temperature_CallBack;
typedef  std::function<void(unsigned char* _buffer, int _buffer_size)> Stream_CallBack;

namespace SRCam
{
    enum StateType
    {
        None,
        Login,
        LoginError,
        Playing,
        PlayError,
        Close
    };

    //通道信息
    struct stChannelInfo
    {
        //通道号
        int channelIndex;
        //通道编码;
        std::string channelCode;
        //通道名称;
        std::string channelName;
        //通道状态;
        int channelStatus;

        int tcpPort ;
        int streamType;
        int codeId;

        bool isOpen = true;

        StateType status = None;
    };

    struct stCameraInfo
    {
        //ip地址
        std::string ip;
        //用户名
        std::string username;
        //密码
        std::string password;
        //端口
        int port;
        //品牌
        std::string brand;

        std::vector<stChannelInfo> channels;

        bool isOpen = true;

        StateType status;
    };


    //云台操作控制参数;
    struct stPTZControlInfo
    {
        //行为类型[0:开始,1:停止];
        int actionType;
        //水平移动速度;
        float pSpeed;
        //上下移动速度;
        float tSpeed;
        //镜头变背速度;
        float zSpeed;
        //斜线运行速度;
        float ptSpeed;

        stPTZControlInfo()
        {
            actionType=0;
            pSpeed=1.0;
            tSpeed=1.0;
            zSpeed=1.0;
            ptSpeed=1.0;
        }
    };


    //云台操作
    enum PTZType
    {
        //基础操作
        PTZ_ZOOM_IN = 0,        //焦距变大
        PTZ_ZOOM_OUT,           //焦距变小
        PTZ_FOCUS_NEAR,         //焦距前调
        PTZ_FOCUS_FAR,          //焦距后调
        PTZ_UP,                 //云台上扬
        PTZ_DOWN,               //云台下附
        PTZ_LEFT,               //云台左转
        PTZ_RIGHT,              //云台右转
        PTZ_UP_LEFT,            //云台左上
        PTZ_UP_RIGHT,           //云台右上
        PTZ_DOWN_LEFT,          //云台左下
        PTZ_DOWN_RIGHT,         //云台右下

        //预置点操作
        PTZ_SET_PRESET,         //设置预置点;
        PTZ_CLEAR_PRESET,       //清除预置点;
        PTZ_GOTO_PRESET         //转到预置点;
    };

    //云台信息
    struct stPTZInfo
    {
        int pValue;
        int tValue;
        int zValue;
    };


}


#endif
