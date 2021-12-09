
#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include "json/json.h"
#include "../CameraSDK/ICameraDefine.h"

#define CONFIGPATH "./config.json"

class Config {
private:
    Config() {
        readJson();
    }

    ~Config() {};

    Config(const Config &);

    Config &operator=(const Config &);

public:
    static Config &getInstance() {
        static Config instance;
        return instance;
    }

    std::vector<SRCam::stCameraInfo> &getstCameraInfos() {
        return m_cameraInfos;
    }

    std::string getMode(){
        return mode;
    }

    std::string getPushServer()
    {
        return pushServer;
    }
private:
    void writeJson() {
        root.clear();

        Json::Value cameraInfosJson;
        for (int i = 0; i < m_cameraInfos.size(); ++i) {
            cameraInfosJson.append(stCameraInfoToJson(m_cameraInfos[i]));
        }
        root["cameraInfos"] = cameraInfosJson;

        root["Common"]["mode"] = mode;
        root["Common"]["pushServer"] = pushServer;

        std::ofstream outfile;
        outfile.open((CONFIGPATH), std::ios::out | std::ios::trunc);
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

        writer->write(root, &outfile);
    }

    void readJson() {
        root.clear();
        std::ifstream ifs;
        ifs.open(CONFIGPATH);
        if (!ifs)//file not exist
        {
            ifs.close();
            genDemoConfig();
            writeJson();
            return;
        }

        if (!ifs.good())
            return;

        Json::CharReaderBuilder readerBuilder;
        JSONCPP_STRING errs;
        if (!parseFromStream(readerBuilder, ifs, &root, &errs)) {
            printf("prase json err\n");
            return;
        }

        ifs.close();
        m_cameraInfos.clear();
        Json::Value cameraInfosJson = root["cameraInfos"];
        for (int i = 0; i < cameraInfosJson.size(); ++i) {
            SRCam::stCameraInfo stCameraInfo = parseCameraInfo(cameraInfosJson[i]);
            m_cameraInfos.push_back(stCameraInfo);
        }

        mode = root["Common"]["mode"].asString();
        pushServer = root["Common"]["pushServer"].asString();
    }

    SRCam::stChannelInfo parseChannelInfo(Json::Value json) {
        SRCam::stChannelInfo channelInfo;
        channelInfo.channelIndex = json["channelIndex"].asInt();
        channelInfo.tcpPort = json["tcpPort"].asInt();
        channelInfo.streamType = json["streamType"].asInt();
        channelInfo.codeId = json["codeId"].asInt();
        channelInfo.isOpen = json["isOpen"].isNull()? true:json["isOpen"].asBool();
        return channelInfo;
    }

    Json::Value ChannelInfoToJson(SRCam::stChannelInfo channelInfo) {
        Json::Value json;
        json["channelIndex"] = channelInfo.channelIndex;
        json["tcpPort"] = channelInfo.tcpPort;
        json["streamType"] = channelInfo.streamType;
        json["codeId"] = channelInfo.codeId;
        json["isOpen"] = channelInfo.isOpen;
        return json;
    }

    SRCam::stCameraInfo parseCameraInfo(Json::Value json) {
        SRCam::stCameraInfo cameraInfo;
        cameraInfo.ip = json["ip"].asString();
        cameraInfo.username = json["username"].asString();
        cameraInfo.password = json["password"].asString();
        cameraInfo.port = json["port"].asInt();
        cameraInfo.brand = json["brand"].asString();
        cameraInfo.isOpen = json["isOpen"].isNull()? true:json["isOpen"].asBool();

        Json::Value channelJson = json["channels"];
        for (int i = 0; i < channelJson.size(); ++i) {
            cameraInfo.channels.push_back(parseChannelInfo(channelJson[i]));
        }

        return cameraInfo;
    }

    Json::Value stCameraInfoToJson(SRCam::stCameraInfo &stCameraInfo) {
        Json::Value json;
        json["ip"] = stCameraInfo.ip;
        json["username"] = stCameraInfo.username;
        json["password"] = stCameraInfo.password;
        json["port"] = stCameraInfo.port;
        json["brand"] = stCameraInfo.brand;
        json["isOpen"] = stCameraInfo.isOpen;
        Json::Value channelJson;
        for (int i = 0; i < stCameraInfo.channels.size(); ++i) {
            channelJson.append(ChannelInfoToJson(stCameraInfo.channels[i]));
        }

        json["channels"] = channelJson;
        return json;
    }

    void genDemoConfig() {

        SRCam::stCameraInfo cameraInfo;
        cameraInfo.ip = "192.168.1.88";
        cameraInfo.port = 8000;
        cameraInfo.username = "admin";
        cameraInfo.password = "admin";
        cameraInfo.brand = "hk";
        cameraInfo.isOpen = true;

        SRCam::stChannelInfo channelInfo;
        channelInfo.channelIndex = 1;
        channelInfo.streamType = 0;
        channelInfo.tcpPort = 1;
        channelInfo.codeId = 1;
        channelInfo.isOpen = true;
        cameraInfo.channels.push_back(channelInfo);

        m_cameraInfos.push_back(cameraInfo);
    }


private:
    Json::Value root;

    std::vector<SRCam::stCameraInfo> m_cameraInfos;

    std::string mode = "PushMode";
    std::string pushServer = "192.168.1.58";
};

#endif //CONFIG_H
