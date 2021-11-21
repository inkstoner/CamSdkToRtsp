#include "CameraHK.h"
#include "ICameraFunction.h"
#include "../Common/PsHelper.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

#if defined(__linux__)

#include <iconv.h>

int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;
    cd = iconv_open(to_charset, from_charset);
    if (cd == 0) return -1;
    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1) return -1;
    iconv_close(cd);
    *pout = '\0';
    return 0;
}

#endif

void CALLBACK getRealtimeThermometryCallback(DWORD dwType, void *lpBuffer, DWORD dwBufLen, void *pUserData) {
    auto cameraHk = reinterpret_cast<CameraHK *>(pUserData);

    if (dwType == NET_SDK_CALLBACK_TYPE_DATA) {
        LPNET_DVR_THERMOMETRY_UPLOAD lpThermometryInfo = new NET_DVR_THERMOMETRY_UPLOAD;
        memcpy(lpThermometryInfo, lpBuffer, sizeof(*lpThermometryInfo));
        {
            char szStr[512] = {0};
            if (0 == lpThermometryInfo->byRuleCalibType) {
                sprintf(szStr, "点测温");
            } else if (1 == lpThermometryInfo->byRuleCalibType) {
                sprintf(szStr, "区域测温");
            } else if (2 == lpThermometryInfo->byRuleCalibType) {
                sprintf(szStr, "线测温");
            }
            // printf("%s ",szStr);
            if (0 == lpThermometryInfo->byRuleCalibType) {
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%f", lpThermometryInfo->struPointThermCfg.fTemperature);
                //printf("%s ",szStr);
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "x:%f,y:%f", lpThermometryInfo->struPointThermCfg.struPoint.fX,
                        lpThermometryInfo->struPointThermCfg.struPoint.fY);
                // printf("%s ",szStr);
            } else if (1 == lpThermometryInfo->byRuleCalibType || 2 == lpThermometryInfo->byRuleCalibType) {
                std::string fMaxTemperature, fMinTemperature, fAverageTemperature, fTemperatureDiff;
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%f", lpThermometryInfo->struLinePolygonThermCfg.fMaxTemperature);
                //printf("%s ",szStr);
                fMaxTemperature = szStr;
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%f", lpThermometryInfo->struLinePolygonThermCfg.fMinTemperature);
                // printf("%s ",szStr);
                fMinTemperature = szStr;
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%f", lpThermometryInfo->struLinePolygonThermCfg.fAverageTemperature);
                //printf("%s ",szStr);
                fAverageTemperature = szStr;
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%f", lpThermometryInfo->struLinePolygonThermCfg.fTemperatureDiff);
                //printf("%s ",szStr);
                fTemperatureDiff = szStr;
                memset(szStr, 0, sizeof(szStr));
                int iPointNum = lpThermometryInfo->struLinePolygonThermCfg.struRegion.dwPointNum;
                for (int i = 0; i < iPointNum; i++) {
                    float fX = lpThermometryInfo->struLinePolygonThermCfg.struRegion.struPos[i].fX;
                    float fY = lpThermometryInfo->struLinePolygonThermCfg.struRegion.struPos[i].fY;
                    sprintf(szStr, "%sX%d:%f,Y%d:%f;", szStr, i + 1, fX, i + 1, fY);
                    //printf("%s ",szStr);
                }

                cameraHk->setTemperatureData(fMaxTemperature, fMinTemperature, fAverageTemperature, fTemperatureDiff);
            }

            memset(szStr, 0, sizeof(szStr));
            if (0 == lpThermometryInfo->byThermometryUnit) {
                sprintf(szStr, "摄氏度");
            } else if (1 == lpThermometryInfo->byThermometryUnit) {
                sprintf(szStr, "华氏度");
            } else if (2 == lpThermometryInfo->byThermometryUnit) {
                sprintf(szStr, "开尔文");
            }
            //printf("%s ",szStr);
            memset(szStr, 0, sizeof(szStr));
            if (0 == lpThermometryInfo->byDataType) {
                sprintf(szStr, "检测中");
            } else if (1 == lpThermometryInfo->byDataType) {
                sprintf(szStr, "开始");
            } else if (2 == lpThermometryInfo->byDataType) {
                sprintf(szStr, "结束");
            }
            //printf("%s ",szStr);
            if ((lpThermometryInfo->bySpecialPointThermType >> 0) & 0x01) {
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%0.1f", lpThermometryInfo->fCenterPointTemperature);
            }
            if ((lpThermometryInfo->bySpecialPointThermType >> 1) & 0x01) {
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%0.1f", lpThermometryInfo->fHighestPointTemperature);
            }
            if ((lpThermometryInfo->bySpecialPointThermType >> 2) & 0x01) {
                memset(szStr, 0, sizeof(szStr));
                sprintf(szStr, "%0.1f", lpThermometryInfo->fLowestPointTemperature);
            }
            //printf("%s ",szStr);
            memset(szStr, 0, sizeof(szStr));
            sprintf(szStr, "%f-%f", lpThermometryInfo->struHighestPoint.fX, lpThermometryInfo->struHighestPoint.fY);
            // printf("%s ",szStr);
            memset(szStr, 0, sizeof(szStr));
            sprintf(szStr, "%f-%f", lpThermometryInfo->struLowestPoint.fX, lpThermometryInfo->struLowestPoint.fY);
        }


    } else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS) {
        DWORD dwStatus = *(DWORD *) lpBuffer;
        if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS) {
            std::cout << "NET_DVR_GET_REALTIME_THERMOMETRY Get finish" << std::endl;
        } else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED) {
            DWORD dwErrCode = *(DWORD *) ((char *) lpBuffer + 4);

            std::cout << "NET_DVR_GET_REALTIME_THERMOMETRY, Error code " << dwErrCode << std::endl;
        }
    }
}


void CameraHK::init() {
    //Init SDK
    NET_DVR_Init();
    NET_DVR_SetLogToFile(3);
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    //日志输出
    std::string directory = SRCam::currentPath() + "/log/hkCamera/";
    SRCam::createDirectory(directory);
    NET_DVR_SetLogToFile(0, (char *) directory.c_str(), true);
}

void CameraHK::clean() {
    NET_DVR_Cleanup();
}

bool CameraHK::login(std::string ip, std::string username, std::string password, int port) {
    if (m_loginID >= 0) {
        return true;
    }
    m_cameraInfo.ip = ip;
    m_cameraInfo.username = username;
    m_cameraInfo.password = password;
    m_cameraInfo.port = port;
    m_cameraInfo.brand = "hk";

    memset(&m_deviceInfo, 0, sizeof(NET_DVR_DEVICEINFO_V30));//初始化值
    m_loginID = NET_DVR_Login_V30((char *) ip.c_str(), port, (char *) username.c_str(), (char *) password.c_str(),
                                  &m_deviceInfo);
    if (m_loginID < 0) {
        m_errorCode = NET_DVR_GetLastError();
        return false;
    }
    return true;
}

bool CameraHK::loginOut() {
    if (m_loginID >= 0) {
        NET_DVR_Logout(m_loginID);
        m_loginID = -1;
    }
    return false;
}

bool CameraHK::capturePicture(int channelIndex, std::string &path) {

    std::string directory = SRCam::currentPath() + "/temp/cap/";
    std::string savePath = SRCam::createFilePath(directory,
                                                 "capture_hk_" + m_cameraInfo.ip + "_" + std::to_string(channelIndex) +
                                                 ".jpg");

    LPNET_DVR_JPEGPARA jpegpara = new NET_DVR_JPEGPARA;
    jpegpara->wPicSize = 0xff;//0xff
    jpegpara->wPicQuality = 0;;

    bool statusHandle = NET_DVR_CaptureJPEGPicture(m_loginID, channelIndex, jpegpara, (char *) savePath.c_str());
    if (!statusHandle) {
        jpegpara->wPicSize = 0;
        statusHandle = NET_DVR_CaptureJPEGPicture(m_loginID, channelIndex, jpegpara, (char *) savePath.c_str());
        if (!statusHandle) {
            savePath.clear();
            m_errorCode = NET_DVR_GetLastError();
        }
        return false;
    }
    path = savePath;
    return true;
}


std::vector<SRCam::stChannelInfo> CameraHK::aliveChannel() {
    std::vector<SRCam::stChannelInfo> channelList;

    NET_DVR_IPPARACFG_V40 IpAccessCfg;
    memset(&IpAccessCfg, 0, sizeof(IpAccessCfg));
    DWORD dwReturned = 0;
    bool ipRet = NET_DVR_GetDVRConfig(m_loginID, NET_DVR_GET_IPPARACFG_V40, 0, &IpAccessCfg,
                                      sizeof(NET_DVR_IPPARACFG_V40), &dwReturned);
    if (!ipRet) {
        for (int i = 0; i < MAX_ANALOG_CHANNUM; i++) {
            if (i < m_deviceInfo.byChanNum) {
                SRCam::stChannelInfo info;
                info.channelStatus = 1;
                info.channelIndex = i + m_deviceInfo.byStartChan;
                info.channelName = this->channelName(info.channelIndex);
                channelList.push_back(info);
            }
        }
    } else {
        for (int i = 0; i < MAX_CHANNUM_V30; i++) {
            if (IpAccessCfg.struStreamMode[i].uGetStream.struChanInfo.byChannel > 0)  //ip通道在线
            {
                SRCam::stChannelInfo info;
                info.channelStatus = IpAccessCfg.struStreamMode[i].uGetStream.struChanInfo.byEnable;
                info.channelIndex = i + IpAccessCfg.dwStartDChan;
                info.channelName = this->channelName(info.channelIndex);
                channelList.push_back(info);
            }

        }
    }
    return channelList;
}

std::string CameraHK::channelName(int channelIndex) {
    std::string channelName;
    DWORD dwReturned = 0;
    NET_DVR_PICCFG_V40 devcfg;
    devcfg.dwSize = sizeof(NET_DVR_PICCFG_V40);
    if (NET_DVR_GetDVRConfig(m_loginID, NET_DVR_GET_PICCFG_V40, channelIndex, &devcfg, sizeof(NET_DVR_PICCFG_V40),
                             &dwReturned)) {
        char *p = new char[sizeof(devcfg.sChanName)];
        memcpy(p, devcfg.sChanName, sizeof(devcfg.sChanName));
        p[sizeof(devcfg.sChanName)] = 0;
        char outStr[128] = {0};
#if defined(__linux__)
        code_convert("gb2312", "utf-8", p, strlen(p), outStr, 128);
#endif
        channelName = outStr;
        delete[] p;
    }
    return channelName;
}


bool CameraHK::ptzControl(int channelIndex, SRCam::PTZType ptzType, SRCam::stPTZControlInfo controlInfo) {
    bool suc;
    switch (ptzType) {
        case SRCam::PTZ_ZOOM_IN:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, ZOOM_IN, controlInfo.actionType,
                                                    controlInfo.zSpeed);
            break;
        case SRCam::PTZ_ZOOM_OUT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, ZOOM_OUT, controlInfo.actionType,
                                                    controlInfo.zSpeed);
            break;
        case SRCam::PTZ_FOCUS_NEAR:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, FOCUS_NEAR, controlInfo.actionType,
                                                    controlInfo.zSpeed);
            break;
        case SRCam::PTZ_FOCUS_FAR:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, FOCUS_FAR, controlInfo.actionType,
                                                    controlInfo.zSpeed);
            break;
        case SRCam::PTZ_UP:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, TILT_UP, controlInfo.actionType,
                                                    controlInfo.tSpeed);
            break;
        case SRCam::PTZ_DOWN:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, TILT_DOWN, controlInfo.actionType,
                                                    controlInfo.tSpeed);
            break;
        case SRCam::PTZ_LEFT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, PAN_LEFT, controlInfo.actionType,
                                                    controlInfo.pSpeed);
            break;
        case SRCam::PTZ_RIGHT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, PAN_RIGHT, controlInfo.actionType,
                                                    controlInfo.pSpeed);
            break;
        case SRCam::PTZ_UP_LEFT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, UP_LEFT, controlInfo.actionType,
                                                    controlInfo.ptSpeed);
            break;
        case SRCam::PTZ_UP_RIGHT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, UP_RIGHT, controlInfo.actionType,
                                                    controlInfo.ptSpeed);
            break;
        case SRCam::PTZ_DOWN_LEFT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, DOWN_LEFT, controlInfo.actionType,
                                                    controlInfo.ptSpeed);
            break;
        case SRCam::PTZ_DOWN_RIGHT:
            suc = NET_DVR_PTZControlWithSpeed_Other(m_loginID, channelIndex, DOWN_RIGHT, controlInfo.actionType,
                                                    controlInfo.ptSpeed);
            break;
    }
    if (!suc) {
        m_errorCode = NET_DVR_GetLastError();
    }
    return suc;
}

bool CameraHK::presetControl(int channelIndex, SRCam::PTZType ptzType, int presetIndex) {
    bool suc;
    switch (ptzType) {
        case SRCam::PTZ_SET_PRESET:
            suc = NET_DVR_PTZPreset_Other(m_loginID, channelIndex, SET_PRESET, presetIndex);
            break;
        case SRCam::PTZ_CLEAR_PRESET:
            suc = NET_DVR_PTZPreset_Other(m_loginID, channelIndex, CLE_PRESET, presetIndex);
            break;
        case SRCam::PTZ_GOTO_PRESET:
            suc = NET_DVR_PTZPreset_Other(m_loginID, channelIndex, GOTO_PRESET, presetIndex);
            break;
    }
    if (!suc) {
        m_errorCode = NET_DVR_GetLastError();
    }
    return suc;
}

bool CameraHK::startThermometry(int cameraChannel, Temperature_CallBack temperatureCallBack) {
    NET_DVR_REALTIME_THERMOMETRY_COND m_struCond = {0};
    m_struCond.dwSize = sizeof(NET_DVR_REALTIME_THERMOMETRY_COND);
    m_struCond.dwChan = cameraChannel;
    m_struCond.byMode = 1;
    m_struCond.byRuleID = 0;
    m_temperatureCallBack = temperatureCallBack;
    m_lRealTimeInfoHandle = NET_DVR_StartRemoteConfig(m_loginID, NET_DVR_GET_REALTIME_THERMOMETRY, &m_struCond,
                                                      sizeof(m_struCond), getRealtimeThermometryCallback, this);
    if (m_lRealTimeInfoHandle >= 0) {
        return true;
    }
    return false;
}

bool CameraHK::stopThermometry() {
    if (m_lRealTimeInfoHandle >= 0) {
        bool res = NET_DVR_StopRemoteConfig(m_lRealTimeInfoHandle);
        if (res) {
            m_lRealTimeInfoHandle = -1;
            return true;
        }
        m_errorCode = NET_DVR_GetLastError();
    }
    return false;
}

void
CameraHK::setTemperatureData(std::string fMaxTemperature, std::string fMinTemperature, std::string fAverageTemperature,
                             std::string fTemperatureDiff) {
    if (m_temperatureCallBack) {
        m_temperatureCallBack(fMaxTemperature, fMinTemperature, fAverageTemperature, fTemperatureDiff);
    }
}

void CALLBACK CameraHK::real_data_cb(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser) {
    auto pThis = reinterpret_cast<PsHelper *>(pUser);
    switch (dwDataType) {
        case NET_DVR_STREAMDATA:
            pThis->save_ps_stream(pBuffer, dwBufSize);
            break;
        default:
            break;
    }
}

bool CameraHK::captureStream(int channelIndex, Stream_CallBack _cb, void *_user_data, long &_capture_handle,
                             int streamType) {
//    if (_cb == NULL)
//        return false;

    NET_DVR_PREVIEWINFO preview_info;
    memset(&preview_info, 0, sizeof(preview_info));
    preview_info.hPlayWnd = NULL;
    preview_info.lChannel = channelIndex;
    preview_info.dwStreamType = streamType;
    preview_info.dwLinkMode = 0;
    _capture_handle = NET_DVR_RealPlay_V40(m_loginID, &preview_info, real_data_cb, _user_data);
    if (_capture_handle == -1) {
        LONG error = NET_DVR_GetLastError();
        printf("NET_DVR_RealPlay_V40 start_capture failed, _capture_cnl(%d), error(%d): %d",
               channelIndex, error, NET_DVR_GetErrorMsg(&error));
        return false;
    }
    return true;
}

