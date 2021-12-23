# CamSdkToRtsp
主要功能
----------
- 1、用于海康摄像头SDK取流转rtsp 、rtmp 、flv
- 2、内网视频转外网输出
- 3、支持自建rtspserver，输出rtsp，由于源码淘宝买板送的，不知道协议，就不放了。

Build：
----------
    先编译[ZLMediaKit](https://github.com/ZLMediaKit/ZLMediaKit)
    
    sudo apt-get install -y libmp3lame-dev libopus-dev  
    mkdir build && cd build  
    cmake .. && make

Run:
----------
    运行ZLMediaKit   

    ./SDKTORTSP  
    第一次未生成json配置文件，会生成一个示例配置文件，配置含义见字段名


More：
----------
- 1、任务管理器，对任务进行定时管理，如果通道长时间未获取到视频流，就重启通道
- 2、如果设备长时间未获取到流，就重新登录设备。
- 3、其他摄像头类型支持，如大华、宇视、天地伟业等。
- 4、rtsp推流RTMP\FLV也可以用此代码稍微改下

由于比较小众，就暂不补充以上功能，需要的人自行完成

参考使用源码：  
    <https://github.com/leixiaohua1020/simplest_ffmpeg_mem_handler>  
    <https://github.com/leixiaohua1020/simplest_ffmpeg_streamer>  
    感谢雷神!!!  

    非常轻型的TS和PS封装与解封装代码
    <https://github.com/xphh/litets>

三方库
    jsoncpp、ffmpeg、zlog
