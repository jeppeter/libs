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

sub IsStringCmpNoSuffix($$$)
{
	my ($astr,$bstr,$suffix)=@_;

	if ($astr eq $bstr ||
		$astr.$suffix eq $bstr  ||
		$astr eq $bstr.$suffix)
	{
		return 0;
	}

	if ($astr gt $bstr)
	{
		return 1;
	}

	return -1;	

}



sub FindInArray($@)
{
	my ($elem,@arr)=@_;
	my ($i,$j,$k,$cmp);

	$i = 0;
	$j = scalar(@arr);
	if ($j < 1)
	{
		return 0;
	}

	$j -- ;

	while($i < $j)
	{
		if ($i == ($j -1))
		{
			$cmp = IsStringCmpNoSuffix($elem,$arr[$i],":amd64");
			if ($cmp == 0)
			{
				return 1;
			}
			$cmp = IsStringCmpNoSuffix($elem,$arr[$j],":amd64");
			if ($cmp == 0)
			{
				return 1;
			}
			return 0;
		}

		$k = int(($i+$j)/2);
		if ($k == $i || $k == $j)
		{
			return 0;
		}

		$cmp = IsStringCmpNoSuffix($elem,$arr[$k],":amd64");

		if ($cmp > 0)
		{
			$i = $k;
		}
		elsif ($cmp < 0)
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

sub SortArray(@)
{
	my (@arr) = @_;
	my (@sortarr) = @arr;
	my ($i,$j,$len,$tmp);

	$len = scalar(@arr);
	for ($i=0;$i<$len;$i++)
	{
		for($j=($i+1);$j < $len;$j++)
		{
			if ("$sortarr[$i]" gt "$sortarr[$j]")
			{
				$tmp = $sortarr[$i];
				$sortarr[$i] = $sortarr[$j];
				$sortarr[$j] = $tmp;
			}
		}
	}

	return @sortarr;
}

sub ReadArray($)
{
	my ($fn)= @_;
	my ($fh,$l,@arr);
	my (@rarr);

	if ($fn eq "-")
	{
		$fh = \*STDIN;
	}
	else
	{
		open($fh,"< $fn") || ErrorExit(3,"could not open($fn)");
	}

	while(<$fh>)
	{
		$l = $_;
		chomp($l);
		if ($l =~ m/^#/o)
		{
			next;
		}
		$l =~ s/^\s+//;
		$l =~ s/\s+$//;
		@arr = split(/[\s]+/,$l);
		if (scalar(@arr) > 0)
		{
			push(@rarr,@arr);
		}
	}

	if ($fh != \*STDIN)
	{
		close($fh);
	}
	undef($fh);

	return @rarr;
}

sub ReadArrayCmd($)
{
	my ($fn)= @_;
	my ($fh,$l,@arr);
	my (@rarr);

	open($fh," $fn | ") || ErrorExit(3,"could not open($fn)");

	while(<$fh>)
	{
		$l = $_;
		chomp($l);
		if ($l =~ m/^#/o)
		{
			next;
		}
		$l =~ s/^\s+//;
		$l =~ s/\s+$//;
		@arr = split(/[\s]+/,$l);
		if (scalar(@arr) > 0)
		{
			push(@rarr,@arr);
		}
	}

	close($fh);
	undef($fh);

	return @rarr;
}

sub ArrayEqual($$)
{
	my ($aref,$bref)=@_;
	my ($i,$j);
	my ($alen,$blen);

	$alen = scalar(@{$aref});
	$blen = scalar(@{$bref});

	if ($alen != $blen)
	{
		return 0;
	}

	for ($i=0;$i<$alen;$i++)
	{
		if (FindInArray($aref->[$i],@{$bref})==0)
		{
			return 0;
		}
	}

	for ($i=0;$i<$alen;$i++)
	{
		if (FindInArray($bref->[$i],@{$aref})==0)
		{
			return 0;
		}
	}

	return 1;
}

my (@remainpkgs,@allpkgs);
my ($maxtimes,$i,$j);
my ($remainfile,$allfile,$pn,$output);
my ($curdpkgscmd)="dpkg -l | grep -e \'^ii\' | awk \'{print \$2}\'";
my ($res,$lasti,$hasinstall);


$remainfile = @ARGV >= 1 ?  shift @ARGV : "-";

@remainpkgs = SortArray(ReadArray($remainfile));


@allpkgs = SortArray(ReadArrayCmd($curdpkgscmd));

$maxtimes=scalar(@allpkgs);

for($i=0;$i<$maxtimes;$i++)
{
	my (@leftpkgs);
	my (@oldpkgs)=@allpkgs;
	my ($hasremove,$len,@nextpkgs);

	@allpkgs = SortArray(ReadArrayCmd($curdpkgscmd));
	if (ArrayEqual(\@remainpkgs,\@allpkgs))
	{
		last;
	}

	$len = scalar(@oldpkgs);
	$hasremove = 0;
	for ($j=0;$j<$len;$j++)
	{
		$pn = $oldpkgs[$j];
		if (FindInArray($pn,@remainpkgs)==0)
		{
			print STDOUT "Remove ($pn)";
			flush STDOUT;
			$output = `apt-get -y --purge --auto-remove remove $pn 2>&1`;
			$res = $?;

			if ($res != 0)
			{
				print STDOUT "\e[31m[FAILED]\e[0m\n";
				push(@nextpkgs,$pn);
			}
			else
			{
				print STDOUT "\e[32m[SUCCESS]\e[0m\n";
			}
			flush STDOUT;
			$hasremove ++;
		}
		else
		{
				push(@nextpkgs,$pn);
		}
	}

	@allpkgs = @nextpkgs;

	if ($hasremove == 0)
	{
		last;
	}
}

@allpkgs = SortArray(ReadArrayCmd($curdpkgscmd));


if(ArrayEqual(\@allpkgs,\@remainpkgs)==0)
{
	for ($i=0;$i<scalar(@allpkgs);$i++)
	{
		$pn = $allpkgs[$i];
		if (FindInArray($pn,@remainpkgs)==0)
		{
			print STDOUT "Not Removed ($pn)\n";
			exit(3);
		}		
	}

	$i = 0;
	$j = 0;
	$lasti = 0;	
	$hasinstall = 0;
	while ($i<scalar(@remainpkgs))
	{		
		if ($j > (scalar(@remainpkgs) * 2))
		{
			ErrorExit(3,"try check more than ($j) times");
		}
		$j ++;
		$pn = $remainpkgs[$i];
		if (FindInArray($pn,@allpkgs)==0 )
		{
			$hasinstall ++;
			if ($lasti == $i)
			{
				ErrorExit(3,"last time not install ($pn) succ");
			}
			print STDOUT "Install($pn)";			
			$output = `apt-get -y  install $pn 2>&1`;
			$res = $?;
			if ($res != 0)
			{
				print STDOUT "\e[31m[FAILED]\e[0m\n";
				exit(3);
			}
			print STDOUT "\e[32m[SUCCESS]\e[0m\n";
			@allpkgs = SortArray(ReadArrayCmd($curdpkgscmd));
			$lasti = $i;
		}
		else
		{
			$i ++;
		}
	}

	if ($hasinstall)
	{
		for ($i=0;$i<scalar(@allpkgs);$i++)
		{
			$pn = $allpkgs[$i];
			if (FindInArray($pn,@remainpkgs)==0)
			{
				print STDOUT "Reinstalled ($pn)\n";
				exit(3);
			}
		}
	}
}

print STDOUT "All Success\n";
