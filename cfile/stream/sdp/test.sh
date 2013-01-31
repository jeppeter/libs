#! /bin/sh

files=`ls samples/*.sdp`

for f in $files
do
	./sdp $f >/dev/null
	if [ $? -ne 0  ]
		then
		echo "can not parse $f right"
		exit 3
		fi
	echo "parse $f correct"
done
