#! /bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

cd build
cmake  -DCMAKE_BUILD_TYPE=Debug ..  ##R elease
make

# strip serv