#include "CameraFactory.h"
#include "CameraHK.h"
#include "ICameraFunction.h"
#include <iostream>
ICameraSDK* CameraFactory::createCamera(std::string deviceType)
{
    ICameraSDK* cameraSDK;
    if(deviceType=="hk")
    {
        cameraSDK=new CameraHK();
    }
    else if(deviceType=="dh")
    {
//        cameraSDK=new CameraDH();
    }
    return cameraSDK;
}
ICameraSDK* createCameraLogin(std::string ip, std::string userName, std::string pwd, int port, std::string deviceType,bool& login)
{
    std::string key = ip + "_" + std::to_string(port)+ "_" + userName + "_" + pwd + "_" +deviceType;
    ICameraSDK* cameraSDK;
    if(deviceType=="hk")
    {
        cameraSDK=new CameraHK();
    }
    else if(deviceType=="dh")
    {
//        cameraSDK=new CameraDH();
    }
    login=cameraSDK->login(ip,userName,pwd,port);
    return cameraSDK;
}

void CameraFactory::initCamera(std::vector<std::string> brandList)
{
    if(SRCam::contains(brandList, "hk"))
    {
        CameraHK::init();
    }
    if(SRCam::contains(brandList, "dh"))
    {
//        CameraDH::init();
    }
}

bool CameraFactory::releaseCamera(ICameraSDK* cameraSDK)
{
    bool res=cameraSDK->loginOut();
    if(res)
    {
        delete cameraSDK;
        cameraSDK=NULL;
    }
    return res;
}

