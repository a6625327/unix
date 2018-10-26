#! /bin/sh
dir=log

#if dir is null
if [ "`ls -A ${dir}`" = "" ]; then
  echo "${dir} is indeed empty"
else
    for file in ${dir}/*; do
        # 如果时丢弃的文件日志，则不清空       
        if [[ ${file} != *"discard_file_info.txt"* ]];then
            echo "echo '' > ${file}"
            echo '' > ${file}
        fi

        # 如果包含线程日志，则删除线程日志
        if [[ ${file} == *"thread_log"* ]];then
            rm -vf ${file}
        fi
    done
fi

# 删除可执行文件下的缓存文件
rm -rv bin/tmp*
rm -rv bin/*core*

cd build
make clean