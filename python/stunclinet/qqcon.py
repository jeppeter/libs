#! python

import logging
from optparse import OptionParser
import sys
import PyQQ
import traceback
import signal
import time

Running = 1

def SigHandler(signo,frame):
	global Running
	Running = 0
	return

def HandleMessage(qq,user,msg):
	pass

def MessageListen(user,pwd):
	_lasttime = time.time()
	try:
		qqlisten = PyQQ()
		qqlisten.Login(user,pwd)
	except:
		# we sleep for a while and try next
		logging.error("could not loggin %s %s"%(str(user),str(pwd))
		time.sleep(3)
		return
		
	try:
		users,msgs = qqlisten.GetMessage('','')
		while len(users) > 0:
			_tmpuser = users.pop()
			_tmpmsg = msgs.pop()
			HandleMessage(qqlisten,_tmpuser,_tmpmsg)			
		_curtime = time.time()
		# we keep the alive in 60 times
		# or we have the time changed
		if ( _curtime - _lasttime ) > 60 or (_curtime < _lasttime):
			if (_curtime < _lasttime):
				logging.warning("time changed _cur(%s)  _last(%s)"%(str(_curtime),str(_lasttime)))
			qqlisten.KeepAlive()
			_lasttime = _curtime
		# we sleep for a while
		time.sleep(2)
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
	signal.signal(signal.SIGINT,)
	oparse = OptionParser()
	oparse.add_option('-q','--qq',action="store",dest="qqs",help="qq number set")
	oparse.add_option('-p','--password',action="store",dest="pwds",help="qq password to set,please append it immediate after the -q or --qq")
	oparse.add_option('-P','--port',action="store",type="int",default=3947,dest='port',help="to specify the port of local default is 3947")

	(options,args)=oparse.parse_args()
	if options.qq is None or options.pwds is None :
		Usage(options,3,"Must specify the -q and -p")

	global Running
	while Running == 1:
		MessageListen(options.qq,options.pwds)
	
