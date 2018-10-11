#! /bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

# if [ ! -d "bin" ];then
#     mkdir bin
# else
#     echo "bin文件夹已经存在"
# fi

if [ ! -d "log" ];then
    mkdir log
else
    echo "log文件夹已经存在"
fi

# 日志解析库复制至默认系统库中
sudo cp -rvf libs/iniparser/libiniparser* /usr/local/lib/
sudo ln -s /usr/local/lib/libiniparser.so.1 /usr/local/lib/libiniparser.so

# 使解析库生效
sudo ldconfig

cd build
# Debug则包含调试信息，若要去除多余的调试信息，可以在bin文件夹下对相应的可执行程序执行strip命令
cmake  -DCMAKE_BUILD_TYPE=Debug ..  ##R elease
make

# strip serv