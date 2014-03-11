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


my (@removepkgs);
my ($maxtimes,$i);

while(<>)
{
	my ($l)=$_;
	my (@arr);
	chomp($l);
	$l =~ s/^\s+//;
	$l =~ s/\s+$//;
	@arr = split(/[\s]+/,$l);
	if (scalar(@arr) > 0)
	{
		push(@removepkgs,@arr);
	}
}

foreach (@removepkgs)
{
	my ($p)=$_;
	DebugLine("pacakge $p");
}

$maxtimes=scalar(@removepkgs);

for($i=0;$i<$maxtimes;$i++)
{
	my (@leftpkgs);
	my (@oldpkgs)=@removepkgs;
	if (scalar(@removepkgs) ==0)
	{
		last;
	}

	foreach(@removepkgs)
	{
		my ($p)=$_;
		my ($ret);

		$ret = system("apt-get --yes --purge remove $p >/dev/null 2>&1");
		if ($ret != 0)
		{
			push(@leftpkgs,$p);
			print STDOUT "|$p($ret)|";
			flush STDOUT;
		}
		else
		{
			print STDOUT ".";
			flush STDOUT;
		}
	}
	print STDOUT "\n";

	@removepkgs = @leftpkgs;
	if (scalar(@oldpkgs) == scalar(@removepkgs))
	{
		ErrorExit(3,"could not remove a package");
	}
}

if (scalar(@removepkgs) > 0)
{
	ErrorExit(3,"can not remove @removepkgs");
}
