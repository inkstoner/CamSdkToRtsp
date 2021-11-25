#include <iostream>
#include <vector>
#include <unistd.h>
#include <thread>
#include "CameraSDK/CameraFactory.h"
#include "Common/PsHelper.h"
#include "RtspSrv/StreamDeal.h"
#include "Common/Config.hpp"
#include "Pusher/PushDeal.h"

int main() {
    std::cout << "System Init !" << std::endl;
    std::vector<std::string> brandList;
    brandList.push_back("hk");
    CameraFactory::getInstance()->initCamera(brandList);

    std::vector<SRCam::stCameraInfo> m_cameraInfos = Config::getInstance().getstCameraInfos();
    std::map<int, ICameraSDK *> devMap;

    while (true) {

        for (int i = 0; i < m_cameraInfos.size(); ++i) {
            SRCam::stCameraInfo* cameraInfo = &(m_cameraInfos[i]);
            if (!cameraInfo->isOpen)
                continue;
            ICameraSDK *cameraSDK = NULL;
            if (devMap.count(i) == 0) {
                ICameraSDK *tcameraSDK = CameraFactory::getInstance()->createCamera("hk");
                devMap[i] = tcameraSDK;
                cameraSDK = tcameraSDK;
            } else {
                cameraSDK = devMap[i];
            }
            //如果未登录
            if (cameraInfo->status != SRCam::Login) {
                std::cout << cameraInfo->ip << " not login ,begin login" << std::endl;
                bool bLogin = cameraSDK->login(cameraInfo->ip, cameraInfo->username, cameraInfo->password,
                                               cameraInfo->port);
                if (bLogin) {
                    std::vector<SRCam::stChannelInfo> info = cameraSDK->aliveChannel();
                    cameraInfo->status = SRCam::StateType::Login;
                } else {
                    cameraInfo->status = SRCam::StateType::LoginError;
                    std::cout << cameraInfo->ip << " login error" << std::endl;
                    break;
                }
            }
            for (int j = 0; j < cameraInfo->channels.size(); ++j) {
                if (!cameraInfo->channels[j].isOpen)
                    continue;
                if (cameraInfo->channels[j].status != SRCam::Playing) {
                    long handle = 0;
                    PsHelper *psHelper = NULL;
                    if (Config::getInstance().getMode() == "PushMode") {
                        PushDeal *pushDeal = new PushDeal(cameraInfo->channels[j].tcpPort);
                        psHelper = pushDeal->getPsHelper();
                    } else {
                        StreamDeal *streamDeal = new StreamDeal(cameraInfo->channels[j].tcpPort,
                                                                cameraInfo->channels[j].codeId);
                        psHelper = streamDeal->getPsHelper();
                    }

                    bool ret = cameraSDK->captureStream(cameraInfo->channels[j].channelIndex, NULL, psHelper, handle,
                                                        cameraInfo->channels[j].streamType);
                    if (!ret) {
                        //不删除缓存，只保留状态
                        cameraInfo->channels[j].status = SRCam::PlayError;
                    } else {
                        cameraInfo->channels[j].status = SRCam::Playing;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
            }

        }


        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60));
    }
//    while (true) {
//        sleep(20);
//    }

//    CameraFactory::getInstance()->releaseCamera(cameraSDK);

    return 0;
}
