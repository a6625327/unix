#! /bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

cd build
cmake --build .. --clean-first
make