#! python

import logging
from optparse import OptionParser
import sys
import PyQQ
import traceback

def MessageListen(user,pwd):
	try:
		qqlisten = PyQQ()
		
	except:
		traceback.print_exc(sys.stderr)
	return

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
	oparse.add_option('-q','--qq',action="store",dest="qqs",help="qq number set")
	oparse.add_option('-p','--password',action="store",dest="pwds",help="qq password to set,please append it immediate after the -q or --qq")
	oparse.add_option('-P','--port',action="store",type="int",default=3947,dest='port',help="to specify the port of local default is 3947")

	(options,args)=oparse.parse_args()
	if options.qq is None or options.pwds is None :
		Usage(options,3,"Must specify the -q and -p")

	while True:
		MessageListen(options.qq,options.pwds)
	
