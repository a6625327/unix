#! /bin/sh
dir=log
 
for file in ${dir}/*; do
    name="0x"
    if [[ ${file} == *${name}* ]]
    then
        rm -vf ${file}
    else
        echo "echo '' > ${file}"
        echo '' > ${file}
    fi
done

rm -rv bin/tmp*

cd build
make clean