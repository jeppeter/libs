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

		# now to send message 
		qq1.SendMsg(qq2num,"Send Hello World\nA new line\n\n")
		tries = 0
		userget = None
		msgget = None
		while tries < 3:
			user,msg = qq2.GetMessage('','')
			userget= user.pop()
			if userget :
				msgget = msg.pop()
				break
			tries += 1
			time.sleep(3)
		if msgget is None:
			raise PyQQException('can not get message from %s'%(qq1num))
		logging.info("get %s message %s"%(userget,msgget))
		qq1.KeepAlive()
		qq2.KeepAlive()
	except PyQQException as e:
		sys.stderr.write("Error %s\n"%(str(e)))
		sys.exit(3)

	