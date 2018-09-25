#! /bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

if [ ! -d "bin" ];then
    mkdir bin
else
    echo "bin文件夹已经存在"
fi

if [ ! -d "log" ];then
    mkdir log
else
    echo "log文件夹已经存在"
fi

sudo cp -rvf libs/iniparser/libiniparser* /usr/local/lib/
sudo ln -s /usr/local/lib/libiniparser.so.1 /usr/local/lib/libiniparser.so
sudo ldconfig

cd build
cmake  -DCMAKE_BUILD_TYPE=Debug ..  ##R elease
make

# strip serv