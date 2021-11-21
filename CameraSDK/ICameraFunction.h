/*********************************************************************************
*FileName:				ICameraFunction.h
*
*Author:				simple
*
*Date:					2021-03-10
*
*Description:			海康摄像头;
**********************************************************************************/
#ifndef ICameraFunction_H
#define ICameraFunction_H

#include <string>
#include <vector>
namespace SRCam
{
    //多级目录创建;
    bool createDirectory(std::string directoryPath);
    //生成文件路径;
    std::string createFilePath(std::string directory,std::string filename);
    //获取当前路径;
    std::string currentPath();
    //判断元素是否存在;
    bool contains(std::vector<std::string>& dataList,std::string data);
}

#endif
