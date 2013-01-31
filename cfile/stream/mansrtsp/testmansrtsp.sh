#! /bin/sh

right_files=`ls samples/*.mansrtsp`
err_files=`ls samples/*.err`

for f in $right_files
do
	./mansrtsp $f >/dev/null
	if [ $? -ne 0 ]
	then
		echo "parse ($f) file error" >&2
		exit 3
	fi
done

for f in $err_files
do
	./mansrtsp $f >/dev/null
	if [ $? -eq 0 ]
	then
		echo "should not parse ($f) file right" >&2
		exit 3
	fi
done
