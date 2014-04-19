#! /bin/bash -x

#功能：生成发布debian大包，发布目录为：build/release/linux_debian
#参数说明：
#$1:
#$2:发布路径



help()
{
cat << HELP
release_debian -- release software with so.
Usage: release_debian FILE DIRECTORY [OPTION]
Options:
  -o DIRECTORY                Object directory.
  -f FILE                     Output file.
  -h                          Print this message and exit.

Example: release_with_so grabber ./hello

HELP
exit 1
}

function pause()
{
        read -n 1 -p "$*" INP
        if [ $INP != '' ] ; then
                echo -ne '\b \n'
        fi
}

#脚本所在路径/build
cur_script_dir=$(cd $(dirname $BASH_SOURCE) && pwd)

#发布目录。
opt_o=${cur_script_dir}/release/linux_debian

#可执行文件。
opt_f=./$1

#用户使用的文件名。
soft_name=kndrosim

[ -z "$opt_f" ]  && help
[ -f "$opt_f" ]  || { echo "$opt_f must be a exectable file"; exit 1; } #若$1不是可执行文件
[ -z "$opt_o" ]  && help
[ -d "$opt_o" ]  || { mkdir "$opt_o" || { echo "$opt_o must can be a exist directory or can be created"; exit 1;} } #若$2不是目录

rm $opt_o/* -r

cp $cur_script_dir/release/res_for_debian/* $opt_o/ -r
#mkdir $opt_o/usr/local/kndrosim/res
cp $cur_script_dir/release/res/* $opt_o/usr/local/kndrosim/res -r
cp $opt_f ${opt_o}/usr/bin/
mv ${opt_o}/usr/bin/$1 ${opt_o}/usr/bin/${soft_name}

pushd ${opt_o}

md5sum `find usr -type f`  >DEBIAN/md5sum

cd ..
[ -e ${soft_name} ] && rm ${soft_name}
dpkg -b ${opt_o} ${opt_o}/${soft_name}.deb

popd
