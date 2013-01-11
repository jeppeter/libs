#! perl


sub DecodeFunc($)
{
	my ($func)=@_;
	my ($str);
	# function name is like _ZN16UsageEnvironmentC1ER13TaskScheduler
	$str = "";
	if ($func =~ m/^_Z[NL]([0-9]+)(.*)/o)
	{
		my ($numbytes)=$1;
		my ($leftfunc)=$2;
		my ($iscont,$partfunc);
		if ($func =~ m/^_ZL/o)
		{
			$str .= "static ";
		}
		$str  .= substr($leftfunc,0,$numbytes);
		$leftfunc = substr($leftfunc,$numbytes);
		# now we should get 
		do
		{
			my ($_curstr);
			$iscont = 0;
			$numbytes = 0;
			if ($leftfunc =~ m/^([0-9]+)(.*)/o)
			{
				$numbytes = $1;
				$leftfunc = $2;
				# now we should get the string
				$partfunc = substr($leftfunc,0,$numbytes);
				$str .="::$partfunc";
				$leftfunc = substr($leftfunc,$numbytes);

				if ($leftfunc =~ m/^[0-9]+/o)
				{
					$iscont = 1;
				}
			}
			if ($leftfunc =~ m/^C[0-9]+/o)
			{
				$str .= "::Constructor";
			}
			elsif ($leftfunc =~ m/^D[0-9]+/o)
			{
				$str .= "::Destructor";
			}
		}while($iscont);
		# now all is ok
	}
	else
	{
		$str = $func;
	}
	return $str;
}


sub GetFuncAddrs($$$)
{
	my ($f,$fnref,$addrref)=@_;
	my ($fh);
	my ($curfunc,$curaddrstr,$curaddr);
	##########################################
	#$fnref is the function name push
	#$addrref is the address function
	##########################################
	open($fh,"<$f") || die "can not open $f $!";
	while(<$fh>)
	{
		my ($l)=$_;
		my (@idxs);
		chomp($l);
		@idxs = split(/:/,$l);
		if (@idxs < 1)
		{
			next;
		}
		if ($idxs[0] =~ m/([0-9a-zA-Z]+)[\s]+<([^<]+)>$/o)		
		{
			$curaddrstr = $1;
			$curfunc = $2;
			$curaddr = hex($curaddrstr);
			
			push(@{$fnref},$curfunc);
			push(@{$addrref},$curaddr);	
			
		}
	}
	
	close($fh);

	return ;
	
}


sub GetAddrFuncOffset($$$)
{
	my ($addr,$fnref,$addrref)=@_;
	my ($str,$lastaddr,$curaddr,$lastfunc,$curfunc,$curaddrstr) ;
	my ($i);
	$str = "";

	$lastaddr = 0;
	$curaddr = 0;
	$lastfunc = "";
	$curfunc = "";
	for ($i=0;$i<@{$fnref};$i++)
	{
		$curaddr = $addrref->[$i];
		$curfunc = $fnref->[$i];
		
		if ($addr < $curaddr)
		{
			# find the address in
			my ($offset);
			$offset = $addr - $lastaddr;
			$lastfunc = DecodeFunc($lastfunc);
			$str = sprintf("[%s+0x%x]",$lastfunc,$offset);
			#print "string is $str\n";
			last;
		}
		
		# now to give the next find
		$lastaddr = $curaddr;
		$lastfunc = $curfunc;
	}
	return $str;
}

my ($f)= shift @ARGV;
my (@fn,@addrs);

GetFuncAddrs($f,\@fn,\@addrs);


while(<STDIN>)
{
	my ($l) = $_;
	my (@idxs);
	chomp($l);
	@idxs = split(/ /,$l);
	if (@idxs < 2)
	{
		print "$l\n";
		next;
	}

	if ($idxs[1] =~ m/\[0x([0-9a-fA-F]+)\]/o)
	{
		my ($addr) = $1;
		my ($hexaddr) = hex($addr);
		my ($str);
		$str = GetAddrFuncOffset($hexaddr,\@fn,\@addrs);
		print "$l $str\n";
		next;
	}
	
	print "$l\n";
}
