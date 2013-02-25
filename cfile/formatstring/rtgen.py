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

arglist = list()
(sfmt,sres)=RandomGen(arglist,int(sys.argv[1]))

logging.basicConfig(level=logging.INFO, format='%(levelname)s %(filename)s:%(lineno)d - - %(asctime)s %(message)s', datefmt='[%b %d %H:%M:%S]')
s = ''
s += '-f %s'%(sfmt)
s += ' '
s += ' -r %s '%(sres)
for arg in arglist:
	s += ' '
	s += '%s'%(arg)

print s

