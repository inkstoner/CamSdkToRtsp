//
// Created by Simple on 2021/3/10.
//

#include "ICameraFunction.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include  <iostream>
#include <algorithm>    // std::find
#include <vector>       // std::vector

#endif


bool SRCam::createDirectory(std::string directory)
{
    //路径长度;
    const int max_path_len=512;
    int dirPathLen = directory.length();
    char currentDir[max_path_len] = { 0 };
    for (int i = 0; i < dirPathLen; ++i)
    {
        currentDir[i] = directory[i];
        if (currentDir[i] == '\\' || currentDir[i] == '/')
        {
            //判断路径是否存在
            if (access(currentDir, 0) != 0)
            {
                int ret = mkdir(currentDir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (ret != 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}




std::string SRCam::createFilePath(std::string directory, std::string filename)
{
    SRCam::createDirectory(directory);
    return directory + filename;
}

std::string SRCam::currentPath()
{
    const int max_path_len=1024;
    char curr_path[max_path_len];
    getcwd(curr_path, max_path_len);
    return curr_path;
}

bool SRCam::contains(std::vector<std::string>& dataList, std::string data)
{
    auto iter = std::find(dataList.begin(), dataList.end(), data);
    if (iter != dataList.end())
    {
        return true;
    }
    return false;
}
