#! /usr/bin/perl -w

################################
#   this file is for check out the file for git repository
################################

sub usage(@){
	my (@args)=@_;
	my ($ec);
	my ($str);
	my ($fp)=*STDERR;
	undef($str);
	if (scalar(@args) <1){
		$ec = 0;
	}elsif (scalar(@args) < 2){
		$ec = $args[0];
	}else{
		$str = $args[1];
	}

	if ($ec == 0){
		$fp = *STDOUT;
	}


	if (defined($str)){
		print $fp "$str\n";
	}
	print $fp "Usage:$0 prevdir curdir prevver [curver]\n";
	exit($ec);	
}

sub check_new_file($$$){
	my ($prevver,$curver,$f)=@_;
	my ($fh,$cmd,$l,@arr);
	$cmd = "git diff --numstat $prevver $curver $f ";
	open($fh,"$cmd |") || return 0;

	$l = <$fh>;	
	close($fh);
	undef($fh);

	if (!defined($l) || length($l) ==0){
		return 0;
	}
	
	chomp($l);
	
	@arr = split(/[\s]+/,$l);
	if (scalar(@arr) < 3){
		return 0;
	}
	
	if ($arr[1] eq "0"){
		return 1;
	}
	return 0;
}

sub check_delete_file($$$){
	my ($prevver,$curver,$f)=@_;
	my ($fh,$cmd,$l,@arr);
	$cmd = "git diff --numstat $prevver $curver $f |";
	open($fh,"$cmd") || return 0;

	$l = <$fh>;
	close($fh);
	undef($fh);

	if (!defined($l) || length($l) ==0){
		return 0;
	}

	chomp($l);
	
	@arr = split(/[\s]+/,$l);
	if (scalar(@arr) < 3){
		return 0;
	}
	if ($arr[0] == "0"){
		return 1;
	}
	return 0;
}


#############################
#  now we should check out the file
#############################
sub check_out_file($$$$$){
	my ($dir,$ver,$curver,$prevver,$f)=@_;
	my ($cmd);
	my ($ret,$dd,$df);

	# first to check out the file
	$cmd = "git checkout $ver $f 2>/dev/null";
	$ret = system($cmd);
	if ($ret != 0 ){
		# we should check out whether it is in the previous version
		if ("$ver" eq "$curver"){
			$ret = check_delete_file($prevver,$curver,$f);
			if ($ret > 0){
				return 0;
			}
			return -1;
		}

		# now to check if it is cur
		$ret = check_new_file($ver,$curver,$f);
		if ($ret > 0){
			# this is new ,so we do not do any more
			return 0;
		}
		return -1;		
	}
	$df = "$dir/$f";
	$dd = `dirname $df`;
	$cmd = "mkdir -p $dd ";
	$ret = system($cmd);
	if ($ret != 0){
		return -1;
	}
	$cmd = "cp -f $f $df";
	$ret = system($cmd);
	if ($ret != 0){
		return -1;
	}
	return 0;
}
sub main(@){
	my (@args)=@_;
	my ($prevver,$curver);
	my ($prevdir,$curdir);
	my ($l,$ret);
	my ($allret)=0;
	if (scalar(@args)<3){
		usage(3);
	}

	$prevdir = $args[0];
	$curdir  = $args[1];
	$prevver = $args[2];
	if (scalar(@args) > 3){
		$curver = $args[3];
	}else{
		$curver = "HEAD";
	}

	while(<STDIN>){
		$l = $_;
		chomp($l);
		$ret = check_out_file($prevdir,$prevver,$curver,$prevver,$l);
		if ($ret < 0){
			$allret = 1;
			print STDERR "can not copy ($prevver:$l) to directory($prevdir)\n";
		}

		$ret = check_out_file($curdir,$curver,$curver,$prevver,$l);
		if ($ret < 0){
			$allret = 1;
			print STDERR "can not copy ($curver:$l) to directory($curdir)\n";
		}
	}

	if ($allret==0){
		print "ALL SUCC\n";
	}
	

	return $allret;
}


main(@ARGV);
