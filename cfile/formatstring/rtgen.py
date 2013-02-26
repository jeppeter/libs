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
def RandomGen(arglist,times=10):
	rf = FormatGen.RandomFormatGen()
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
	res = s
	if rpat.search(s):
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
def MakeShellFormat(n,prefix='test_format_string',times=10):
	print '#! python'
	for i in xrange(n):
		arglist = list()
		(sfmt,sres)=RandomGen(arglist,times)
		s = '%s '%(prefix)
		s += '-f %s'%(QuoteBlank(sfmt))
		s += ' '
		s += ' -r %s '%(QuoteBlank(sres))
		for arg in arglist:
			s += ' '
			s += '%s'%(QuoteBlank(arg))
		print s


logging.basicConfig(level=logging.INFO, format='%(levelname)s %(filename)s:%(lineno)d - - %(asctime)s %(message)s', datefmt='[%b %d %H:%M:%S]')
MakeShellFormat(int(sys.argv[1]),times=int(sys.argv[2]))

