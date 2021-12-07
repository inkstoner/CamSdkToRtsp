cmake_minimum_required(VERSION 3.5)
project(SDKToRtsp)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fpermissive")

include_directories(./thirdpart/hikvision_linux/incCn)
link_directories(./thirdpart/hikvision_linux/lib)

include_directories(./thirdpart/rtsp/include)
link_directories(./thirdpart/rtsp/lib)

include_directories(./thirdpart/x86-jsoncpp/include)
link_directories(./thirdpart/x86-jsoncpp/lib)

include_directories(./thirdpart/ffmpeg_linux/include)
link_directories(./thirdpart/ffmpeg_linux/lib)

include_directories(./thirdpart/zlog/include)
link_directories(./thirdpart/zlog/lib)

add_executable(SDKToRtsp main.cpp
        RtspSrv/StreamUnpack.h
        RtspSrv/StreamUnpack.cpp
        Common/Config.hpp
        RtspSrv/StreamDeal.cpp
        RtspSrv/StreamDeal.h
        Common/PsHelper.h
        Common/PsHelper.cpp
        CameraSDK/CameraHK.cpp
        CameraSDK/CameraHK.h
        CameraSDK/ICameraDefine.h
        CameraSDK/ICameraSDK.h
        CameraSDK/ICameraFunction.h
        CameraSDK/ICameraFunction.cpp
        CameraSDK/CameraFactory.h
        CameraSDK/CameraFactory.cpp
        Utils/litets/litets.h
        Utils/litets/misc.c
        Utils/litets/pes.c
        Utils/litets/ps_demux.c
        Utils/litets/ps_stream.c
        Utils/litets/streamdef_le.h
        Utils/litets/ts_demux.c
        Utils/litets/ts_stream.c

        Pusher/PushDeal.cpp
        Pusher/PushDeal.h
        Pusher/FFmpegPusher.h
        Pusher/FFmpegPusher.cpp
        Utils/ring_buffer_s.h
        Utils/spin_mutex.h
        Utils/httplib.h
        )

#set(ffmpeglib libavcodec.a
#        libavformat.a
#        libavutil.a
#        libavdevice.a
#        libavfilter.a
#        libpostproc.a
#        libswresample.a
#        libswscale.a
#        postproc
#        )

set(ffmpeglib avcodec
        avformat
        avutil
        avdevice
        avfilter
        postproc
        swresample
        swscale
        postproc
        )

target_link_libraries(SDKToRtsp  hcnetsdk PlayCtrl HCCore rtsp jsoncpp pthread ${ffmpeglib})