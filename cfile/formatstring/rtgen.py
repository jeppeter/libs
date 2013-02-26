#! python
###########################
#
#
#  this file is to generate the random test
#  this is the case of random
#
#
#
#
#
#
###########################
import FormatGen
import logging
import re
import sys
import time
def RandomGen(arglist,seed=None,times=10):
	rf = FormatGen.RandomFormatGen(seed)
	s = ''
	r = ''
	for i in xrange(times):
		rf.GenValue()
		s += rf.GetFmt()
		arglist.append(rf.GetOpt())
		arglist.append(rf.GetArgv())
		r += rf.GetSv()
	return s ,r
	
def QuoteBlank(s):
	rpat = re.compile('[\s]+')
	qpat = re.compile('\"')
	q2pat = re.compile('\'')
	res = s
	if rpat.search(s) or qpat.search(s) or q2pat.search(s):
		res = '\"'
		for c in s:
			if c == '\'':
				res += '\\\''
			elif c == '\"':
				res += '\\\"'
			else:
				res += c
		res += '\"'
	return res
def MakeShellFormat(n,prefix='./testformat',times=10):
	print '#! /bin/sh'
	seed = time.time()
	for i in xrange(n):
		arglist = list()		
		(sfmt,sres)=RandomGen(arglist,seed,times)
		s = '%s '%(prefix)
		s += '-f %s'%(QuoteBlank(sfmt))
		s += ' '
		s += ' -r %s '%(QuoteBlank(sres))
		for arg in arglist:
			s += ' '
			s += '%s'%(QuoteBlank(arg))
		print s
		seed += (n*times)


logging.basicConfig(level=logging.INFO, format='%(levelname)s %(filename)s:%(lineno)d - - %(asctime)s %(message)s', datefmt='[%b %d %H:%M:%S]')
MakeShellFormat(int(sys.argv[1]),times=int(sys.argv[2]))

