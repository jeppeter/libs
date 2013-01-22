#! python

import os
#os.path.append(".")

from PyQQ import *
import sys
from optparse import OptionParser
import logging
import random
import time
def Usage(opt,exitcode,msg=None):
	fp = sys.stderr
	if exitcode == 0:
		fp = sys.stdout
	if msg is not None:
		fp.write(msg+"\n")
	opt.print_help(fp)
	sys.exit(exitcode)
	
def TestSendAndReceive(qq1,qq2,msg,fuser='',fmsg='',mustsucc=1):
	try:
		qq1.SendMsg(qq2.GetUser(),msg)
		_count = 0
		_rmsg = None
		_ruser = None
		while _count < 3:
			guser,gmsg = qq2.GetMessage(fuser,fmsg)
			if len(guser) > 0:
				_ruser = guser.pop()
				_rmsg = gmsg.pop()
				break
			time.sleep(3)
			_count += 1
		if _rmsg is None and mustsucc == 1:
			raise PyQQException('Can Get %s message'%(str(fuser)))
		elif mustsucc == 0 and _rmsg is not None:
			raise PyQQException('Receive Message %s'%(str(_rmsg)))
		elif mustsucc == 0:
			logging.info('send %s message %s  fuser %s fmsg %s receive message none'%(qq2.GetUser(),msg,fuser,fmsg))
			return
		logging.info('receive user %s message %s '%(_ruser,_rmsg))
	except PyQQException as e:
		if mustsucc == 1:
			logging.error('send %s msg %s get  fuser %s fmsg %s error %s'%(qq2.GetUser(),msg,fuser,fmsg,str(e)))
		else:
			logging.error('send %s msg %s get  fuser %s fmsg %s error %s'%(qq2.GetUser(),msg,fuser,fmsg,str(e)))
	return 	


if __name__ == '__main__':
	logging.basicConfig(level=logging.INFO,format="%(levelname)-8s %(asctime)-12s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
	oparse = OptionParser()
	oparse.add_option('-q','--qq',action="append",dest="qqs",help="qq number set")
	oparse.add_option('-p','--password',action="append",dest="pwds",help="qq password to set,please append it immediate after the -q or --qq")

	(options,args)=oparse.parse_args()
	if options.qqs is None or len(options.qqs) < 2:
		Usage(oparse,3,"Must specify two qqs")
	if options.pwds is None or len(options.pwds) != len(options.qqs):
		Usage(oparse,3,"passwords must according to qqs")
	qq1num =options.qqs[0]
	qq1pwds = options.pwds[0]
	qq2num = options.qqs[1]
	qq2pwds = options.pwds[1]
	try:
		qq1 = PyQQ()
		qq2 = PyQQ()
		qq1.Login(qq1num,qq1pwds)
		qq2.Login(qq2num,qq2pwds)
	except PyQQException as e:
		sys.stderr.write("Error %s\n"%(str(e)))
		sys.exit(3)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','','',1)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','[\d]+','',1)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','[a-z]+','',0)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','','Hello',1)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','','Hello_NoSuch',0)
	TestSendAndReceive(qq1,qq2,'Hello World New\na line\n','','a line',1)

	
