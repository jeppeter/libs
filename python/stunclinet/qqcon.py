#! python

import logging
from optparse import OptionParser
import sys
from PyQQ import *
import traceback
import signal
import time

global Running
Running = 1

def SigHandler(signo,frame):
	global Running
	Running = 0
	return

def HandleConnectStun(cmds):
	resp = ''
	return resp

def HandleListenPort(cmds):
	resp = ''
	return resp

def HandleSendUDP(cmds):
	resp = ''
	return resp

def HandleMessage(qq,user,msg):
	'''
		function : to handle message and handle the
		it will start with 
		OPENVPN REQUEST##...##...##
		the split characters is ##
	'''
	if msg.startswith('OPENVPN REQUEST##'):
		# ok we should handle this
		response = 'OPENVPN RESPONSE##'
		cmds = msg.split('##')
		if len(cmds) >= 2:
			# ok ,this is the commands
			if cmds[1] == 'CONNECTSTUN':
				response += HandleConnectStun(cmds[2:])				
			elif cmds[1] == 'LISTENPORT':
				response += HandleListenPort(cmds[2:])
			elif cmds[1] == 'SENDUDP':
				response += HandleSendUDP(cmds[2:])
			else:
				response += 'Unknow request (%s)'%(msg)
				logging.warning('can not parse request (%s)'%(msg))
		qq.SendMsg(user,response)
	logging.info("Handle %s msg %s"%(user,msg))	
	return

def MessageListen(user,pwd):
	_lasttime = time.time()
	try:
		qqlisten = PyQQ()
		qqlisten.Login(user,pwd)
	except PyQQException as e:
		# we sleep for a while and try next
		logging.error("could not loggin %s %s %s"%(str(user),str(pwd),str(e)))
		return
	logging.info("User %s Pwd %s loggin ok"%(user,pwd))
	try:
		global Running
		# we exit when running is not set
		while Running == 1:
			users,msgs = qqlisten.GetMessage('','')
			while len(users) > 0:
				logging.info("users %s msgs %s"%(repr(users),repr(msgs)))
				_tmpuser = users.pop()
				_tmpmsg = msgs.pop()
				logging.info("message(%s) (%s)"%(_tmpuser,_tmpmsg))
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
			if Running == 1:
				try:
					time.sleep(2)
				except:
					pass
	except:
		traceback.print_exc(sys.stderr)
	qqlisten = None
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
	signal.signal(signal.SIGINT,SigHandler)
	oparse = OptionParser()
	oparse.add_option('-q','--qq',action="store",dest="qq",help="qq number set")
	oparse.add_option('-p','--password',action="store",dest="pwd",help="qq password to set,please append it immediate after the -q or --qq")
	oparse.add_option('-P','--port',action="store",type="int",default=3947,dest='port',help="to specify the port of local default is 3947")

	(options,args)=oparse.parse_args()
	if options.qq is None or options.pwd is None :
		Usage(options,3,"Must specify the -q and -p")

	while Running == 1:
		MessageListen(options.qq,options.pwd)
		if Running == 1:
			try:
			# we sleep a while call next try
				time.sleep(3)
			except:
				pass
	
