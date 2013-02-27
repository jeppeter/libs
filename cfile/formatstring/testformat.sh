#! /bin/sh

RunTestFormat()
{
	_f=$1
	_a=$@
	./testformat $_a
	if [ $? -ne 0 ]
		then
		echo "run ($_a) not right $?" >&2
		exit 3
		fi
}

while read args
do
	RunTestFormat $args
done

