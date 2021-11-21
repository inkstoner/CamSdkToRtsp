#ifndef CameraFactory_H
#define CameraFactory_H
#include <map>

#include "ICameraSDK.h"

class CameraFactory {
public:
    static CameraFactory* getInstance()
    {
        static CameraFactory m_instance;
        return &m_instance;
    }

    void initCamera(std::vector<std::string> brandList);
    //创建摄像头对象并登录
    ICameraSDK* createCameraLogin(std::string ip, std::string userName, std::string pwd, int port, std::string deviceType,bool& login);
    //创建摄像头
    ICameraSDK* createCamera(std::string deviceType);
    //释放摄像头
    bool releaseCamera(ICameraSDK* cameraSDK);
};


#endif
