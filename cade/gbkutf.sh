#! /bin/sh

todir=$1
if [ -z "$todir" ]
then
	todir="."
fi

FILE_LIST=`find $todir  | egrep -e '*\.gbk.c$' -e  '*\.gbk.h$'  `

echo -e "FILE_LIST($todir)\n$FILE_LIST"


for f in $FILE_LIST
do
	fgbkname=${f%.*}
	fext=${f##*.}
	fname=${fgbkname%.*}
	iconv -f cp936 -t utf-8 $f  >$fname.$fext
done

