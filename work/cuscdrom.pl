#! perl

######################################
#    to make the custom livecd making script
#    this file is following the manual of 
#    https://help.ubuntu.com/community/LiveCDCustomization
######################################


use strict;
use warnings;

# these are the options parser
use Getopt::Std;

use constant {
	SUDO_PREFIX => "sudo "
};


sub debug_str($)
{
	my ($str)=@_;
	my ($p,$f,$l)=caller(0);
	print STDOUT "$f:$l\t$str\n";
}

sub run_cmd($)
{
    my ($cmd) = @_;
    my ($ret);

	debug_str("run cmd($cmd)");
    $ret = system($cmd);
    if ($ret != 0)
    {
        die "could not run($cmd) right\n";
    }
    return 0;

}


sub usage($$)
{
	my ($ec,$str)=@_;
	my ($fp);

	$fp = \*STDERR;
	if ($ec == 0)
	{
		$fp = \*STDOUT;
	}

	if (defined($str))
	{
		print $fp "$str\n";
	}

	print($fp "$0 [OPTIONS]\n");
	print($fp "\t-h          to display this help information\n");
	print($fp "\t-s   dir    to specify the squash file system directory\n");
	print($fp "\t-o   dir    to specify the oldiso directory\n");
	print($fp "\t-n   dir    to specify the newiso directory\n");
	print($fp "\t-i   name   to specify the new iso name\n");
	exit($ec);
}

my ($oldisodir,$newisodir,$squashfsdir,$isoname,$installdir);
our ($opt_h,$opt_s,$opt_o,$opt_n,$opt_i);

undef($oldisodir);
undef($newisodir);
undef($squashfsdir);
undef($isoname);

sub parse_param(@)
{
	my (@params)=@_;
	my ($ret,$str);

	$ret = getopts("hs:o:n:i:");

	undef($str);
	if (defined($opt_h))
	{
		usage(0,$str);
	}

	if (defined($opt_s))
	{
		$squashfsdir=$opt_s;
	}

	if (defined($opt_o))
	{
		$oldisodir=$opt_o;
	}

	if (defined($opt_n))
	{
		$newisodir=$opt_n;
	}

	if (defined($opt_i))
	{
		$isoname=$opt_i;
	}

	if (! defined($squashfsdir))
	{
		usage(3,"must specify squashfs by -s");
	}

	if (! defined($newisodir))
	{
		usage(3,"must specify newisodir by -n");
	}

	if (! defined($isoname))
	{
		usage(3,"must specify isoname by -i");
	}

	return 0;	
}

sub get_newiso_installdir($)
{
	my ($niso)=@_;
	my ($rdir);

	if ( -f "$niso/install/vmlinuz" )
	{
		$rdir = `readlink -f "$niso/install"`;
		chomp($rdir);
		return $rdir;
	}

	if ( -f "$niso/capser/vmlinuz" )
	{
		$rdir=`readlink -f "$niso/capser"`;
		chomp($rdir);
		return $rdir;
	}

	die "($niso) not valid iso directory for ubuntu";
}

sub check_and_umount($)
{
	my ($d)=@_;
	my ($hasmount,$cmd);
	$cmd = SUDO_PREFIX;
	$cmd .= "mount | grep $d ";
	$hasmount=`$cmd`;
	chomp($hasmount);
	if (length($hasmount) > 0)
	{
		$cmd = SUDO_PREFIX;
		$cmd .= "umount $d";
		run_cmd($cmd);
	}
	return ;
}


sub make_squashfs($$)
{
	my ($squashdir,$nisoinstdir)=@_;
	my ($hasmount);
	my ($cmd,$d);

	# now check for 
	$d = "$nisoinstdir/dev/pts";
	check_and_umount($d);
	$d = "$nisoinstdir/dev";
	check_and_umount($d);
	$d = "$nisoinstdir/sys";
	check_and_umount($d);
	$d = "$nisoinstdir/proc";
	check_and_umount($d);

	# to clean up 

	$cmd = SUDO_PREFIX;
	$cmd .= "rm -f $nisoinstdir/filesystem.squashfs";
	run_cmd($cmd);

	$cmd = SUDO_PREFIX;
	$cmd .= "mksquashfs $squashdir $nisoinstdir/filesystem.squashfs -b 1048576";
	run_cmd($cmd);

	return ;	
}

sub make_md5sum($)
{
	my ($nisodir)=@_;
	my ($cmd);

	$cmd = "cd $nisodir";
	$cmd .= " && ";
	$cmd .= SUDO_PREFIX;
	$cmd .= "rm -f md5sum.txt";
	$cmd .= " && ";
	$cmd .= "find . -type f -print0 |";
	$cmd .= SUDO_PREFIX;
	$cmd .= " xargs -0 md5sum | grep -v isolinux/boot.cat |";
	$cmd .= SUDO_PREFIX ;
	$cmd .= " tee md5sum.txt >/dev/null";
	run_cmd($cmd);
	return ;
	
		
}

sub make_manifest($$)
{
	my ($squashdir,$nisoinstdir)=@_;
	my ($cmd);

	$cmd = SUDO_PREFIX;
	$cmd .= "chmod +w $nisoinstdir/filesystem.manifest";
	run_cmd($cmd);

	$cmd = SUDO_PREFIX;
	$cmd .= "chroot $squashdir dpkg-query -W --showformat='\${Package} \${Version}\n' ";
	$cmd .= " | ";
	$cmd .= SUDO_PREFIX;
	$cmd .= " tee $nisoinstdir/filesystem.manifest >/dev/null";
	run_cmd($cmd);	
}

sub make_size($$)
{
	my ($nisodir,$ninstdir)=@_;
	my ($cmd);

	$cmd = "printf \$(";
	$cmd .= SUDO_PREFIX;
	$cmd .= "du -sx --block-size=1 $nisodir | cut -f1) | ";
	$cmd .= SUDO_PREFIX;
	$cmd .= "tee $ninstdir/filesystem.size >/dev/null";
	run_cmd($cmd);
	return ;
	
}

sub make_iso($$)
{
	my ($nisodir,$name)=@_;
	my ($cmd,$fname,$fdir,$oname);
	$fname=`readlink -f $name`;
	chomp($fname);
	$fdir=`readlink -f $nisodir`;
	chomp($fdir);
	$oname=`basename $name`;
	chomp($oname);
# sudo mkisofs -D -r -V "bignte-produce-cdrom" -cache-inodes -J -l -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -o ../ubuntu-14.04-server-amd64-bingte.iso .
	$cmd = "cd $nisodir && ";
	$cmd .= SUDO_PREFIX;
	$cmd .= "mkisofs -D -r -V \"cdrom\" -cache-inodes -J -l";
	$cmd .= " -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table";
	$cmd .= " -o ../$oname .";
	run_cmd($cmd);
	return ;
	
}

parse_param(@ARGV);

$installdir=get_newiso_installdir($newisodir);
make_manifest($squashfsdir,$installdir);
make_squashfs($squashfsdir,$installdir);
make_size($newisodir,$installdir);
make_md5sum($installdir);
make_iso($newisodir,$isoname);



