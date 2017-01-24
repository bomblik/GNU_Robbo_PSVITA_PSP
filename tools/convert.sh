#!/bin/sh
offset=508C
#sometimes this one helps (for K-robbo for eg)
offset=5096
#original offest
offset=388c
#robbo konstruktor offser
#offset=508c

echo "[name]"
echo $3
echo "[last_level]"
echo $2
echo 
zcat $1 >$1.unz
for x in `seq 1 $2` ; do
./lvldump -o $offset -l ${x} -a "$3" -f $1.unz
done 
rm $1.unz
