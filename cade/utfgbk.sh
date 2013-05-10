#! /bin/sh

todir=$1
if [ -z "$todir" ]
then
	todir="."
fi

FILE_LIST=`find $todir  | egrep -e '*\.c$' -e  '*\.h$' | egrep -v '*\.gbk\.c$' | egrep -v '*\.gbk\.h$' `

echo -e "FILE_LIST($todir)\n$FILE_LIST"

for f in $FILE_LIST
do
	fname=${f%.*}
	fext=${f##*.}
	iconv -f utf-8 -t cp936 $f  >$fname.gbk.$fext
done
