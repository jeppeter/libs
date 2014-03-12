#! perl -w

use strict;
use warnings;

sub DebugLine(@)
{
	my (@args) = @_;	
	my ($p,$f,$l) = caller(0);
	print "$f:$l\t";
	foreach(@args)
	{
		print "$_";
	}
	print "\n";
	return;
}

sub ErrorExit($$)
{
	my ($p,$f,$l) = caller(0);
	my ($e,$s)=@_; 

	print STDERR "$f:$l\t Error($s) exit\n";
	exit($e);
}

sub GetModuleUsed()
{
	my (@modules,@arr);
	my ($l,$n,$k);
	my ($f);

	undef($f);

	open($f," lsmod | ") || ErrorExit(3,"could not open lsmod");
	$n = 0;

	while(<$f>)
	{
		$l = $_;
		$n ++;
		chomp($l);
		if ($n == 1)
		{
			# skip the header
			next;
		}

		@arr = split(/[\s]+/,$l);
		if (@arr < 2)
		{
			DebugLine("[$n]$l (<2)");
			next;
		}

		$k = $arr[0].".ko";
		push(@modules,$k);
	}

	close($f);
	undef($f);
	return @modules;
}

sub MatchInArrays($@)
{
	my ($f,@files)=@_;
	my ($i,$j,$k);

	$i = 0;
	$j = scalar(@files);
	while($i < $j)
	{
		$k = int((($i+$j)/2));
		if ($k == $i || $k == $j)
		{
			last;
		}
		if ($f gt "$files[$k]")
		{
			$i = $k;
		}
		elsif ($f lt "$files[$k]")
		{
			$j = $k;
		}
		else
		{
			return 1;
		}
	}

	return 0;
	
}

sub RemoveModules($@)
{
	my ($kernver,@sortmods)=@_;
	my ($i,$j,$tm,$len,$c,$l,$fn,$dn,@arr);
	$len = scalar(@sortmods);
	for ($i=0;$i<$len;$i++)
	{
		for($j=($i+1);$j<$len;$j++)
		{
			if ("$sortmods[$i]" ge "$sortmods[$j]")
			{
				$tm = $sortmods[$i];
				$sortmods[$i] = $sortmods[$j];
				$sortmods[$j] = $tm;
			}
		}
	}

	
	$dn = "/lib/modules/$kernver/";
	DebugLine("find $dn");
	open($c," find $dn -type f |") || ErrorExit(3,"could not find $dn file");
	
	while(<$c>)
	{
		$l = $_;
		chomp($l);
		@arr = split(/\//,$l);
		if (@arr < 2)
		{
			DebugLine("$l (<2)");
			next;
		}

		if ($arr[-1] =~ m/\.ko$/o)
		{
			if (MatchInArrays($arr[-1],@sortmods)==0)
			{
				unlink($l);
			}
		}
	}

	return;
}

# now first to get the version number
my ($kernver,@modused);
my (@notdelmods)=@ARGV;

$kernver=`uname -r`;
chomp($kernver);
DebugLine("kernver $kernver");

@modused = GetModuleUsed();
@modused += notdelmodes;
RemoveModules($kernver,@modused);

