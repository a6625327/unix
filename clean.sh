#! /bin/sh
dir=log

#if dir is null
if [ "`ls -A ${dir}`" = "" ]; then
  echo "${dir} is indeed empty"
else
    for file in ${dir}/*; do
        name="0x"
        if [[ ${file} == *${name}* ]]
        then
            rm -vf ${file}
        else
            if [[ ${file} != *"discard_file_info.txt"* ]];then
                echo "echo '' > ${file}"
                echo '' > ${file}
            fi
        fi
    done
fi

rm -rv bin/tmp*

cd build
make clean