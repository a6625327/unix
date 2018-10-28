#! /bin/sh
# 该脚本清除日志文件以及编译产生的中间文件、core文件、传输接收并且储存在硬盘的文件，该脚本需谨慎使用
# 如果仅需要清除中间编译文件，直接执行#1下的两句就可以
# 并且执行该脚本需要使用bash，不能使用sh
dir=log

#if dir is null
if [ "`ls -A ${dir}`" = "" ]; then
  echo "${dir} is indeed empty"
else
    for file in ${dir}/*; do
        # 如果是丢弃的文件日志，则不清空       
        if [[ ${file} != *"discard_file_info.txt"* ]];then # 注释这行会清空丢弃文件日志
            echo "echo '' > ${file}"
            echo '' > ${file}
        fi # 注释这行会清空丢弃文件日志

        # 如果包含线程日志，则删除线程日志
        if [[ ${file} == *"thread_log"* ]];then
            rm -vf ${file}
        fi
    done
fi

# 删除可执行文件下的缓存文件
rm -rv bin/tmp*
rm -rv bin/*core*

#
cd build
make clean