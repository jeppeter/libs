#! /bin/sh

usage()
{
	_ec=$1
	_fmt=$2

	_outf=/proc/self/fd/2
	if [ $_ec -eq 0 ]
	then
		_outf=/proc/self/fd/1
	fi

	if [ ! -z "$_fmt" ]
	then
		echo "$_fmt"  >$_outf
	fi

	echo "$0 [OPTIONS] dev" >$_outf
	echo -e "\t-h             to display this message" >$_outf
	echo -e "\t-s ssh_command to specify ssh command" >$_outf
	exit $_ec
}

while getopts "hs:" optname 
do
	case $optname in
		h)
		usage 0
		;;
		s)
		_sshcmd=$OPTARG
		;;
		*)
		echo "devname $optname"
		_devname=$optname
		;;
	esac
done

shift $(( OPTIND - 1 ))
_devname=$1

if [ -z "$_devname" ]
then
	usage 3 "must specify devname"
fi

if [ ! -z "$_sshcmd" ]
then
	echo "sshcmd ($_sshcmd)" 
	echo "devname ($_devname)"
	sudo dd if=$_devname | gzip -9 | eval "$_sshcmd"
fi
