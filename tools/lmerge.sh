#!/bin/bash
#
# this script is supposed to be used to merge levelsets into one
# written by neurocyp (at) gmail (dot) com
#
#
maxlevel=`cat $* |grep '\[level\]' |wc -l`

cat $*  | ./lmerge.awk -v lastlevel=${maxlevel} >merge.dat
