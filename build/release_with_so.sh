#! /bin/bash -x

#$1: full running script; $2:release file dir name;

soft_name=kndrosim

help()
{
cat << HELP
release_with_so -- release software with so.
Usage: make FILE DIRECTORY [OPTION]
Options:
  -o DIRECTORY                Object directory.
  -f FILE                     Output file.
  -h                          Print this message and exit.

Example: release_with_so grabber ./hello

HELP
exit 1
}

creatReadme()
{
pushd $1 #不能cd，会影响调用程序

echo "
1.运行根目录下，执行./${soft_name}
2.新建工程,菜单project/new
3.加载res目录下.xml模型文件.菜单robot/load">README.txt
popd
}

creat_running_script()
{
pushd $1
echo '#! /bin/bash
export export  LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./grabber'>${soft_name}
chmod +x ${soft_name}
popd
}

creat_install_script()
{
pushd $1
echo "#! /bin/bash
sudo cp ${soft_name} /usr/bin/
read -n1 -p "Press any key to continue..."
">install
chmod +x install
popd
}

creat_unstall_script()
{
pushd $1
echo "#! /bin/bash
sudo rm /usr/bin/${soft_name}
read -n1 -p "Press any key to continue..."
">unstall
chmod +x unstall
popd
}

function pause()
{
        read -n 1 -p "$*" INP
        if [ $INP != '' ] ; then
                echo -ne '\b \n'
        fi
}

pwd

#脚本所在路径
cur_script_dir=$(cd $(dirname $BASH_SOURCE) && pwd)
#发布目录。
opt_o=${cur_script_dir}/release/$2
#可执行文件。
opt_f=./$1

[ -z "$opt_f" ]  && help
[ -f "$opt_f" ]  || { echo "$opt_f must be a exectable file"; exit 1; } #若$1不是可执行文件
[ -z "$opt_o" ]  && help
[ -d "$opt_o" ]  || { mkdir "$opt_o" || { echo "$opt_o must can be a exist directory or can be created"; exit 1;} } #若$2不是目录

rm $opt_o/* -r

mkdir $opt_o/doc
creatReadme $opt_o/doc

mkdir $opt_o/res
cp $cur_script_dir/release/res/* $opt_o/res

deplist=$( ldd $opt_f | awk '{if (match($3,"/")){ print $3}}' )
cp $deplist $opt_o
cp $opt_f $opt_o
creat_running_script $opt_o
creat_install_script $opt_o
creat_unstall_script $opt_o
[ -e ${opt_o}/../${soft_name}_with_so.tar.gz ] && rm ${opt_o}/../${soft_name}_with_so.tar.gz

pushd $opt_o/
pwd
tar czf ${soft_name}_with_so.tar.gz *  #--remove-files
popd
