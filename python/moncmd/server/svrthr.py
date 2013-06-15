#! python

import threading
import LocalException
import select
import time

class MonSvrThread(threading.Thread):
	def __init__(self,port):
		threading.Thread.__init__(self)
		self.__port = port
		self.__socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		self.__socket.bind(('',port))
		self.__socket.listen(5)
		self.__socket.setblocking(0)
		self.__running = 1
		self.__mutex = threading.Lock()
		self.__clients = []

	def __GetRead(self,timeout):
		rlist = []
		wlist = []
		xlist = []
		rlist.append(self.__socket)
		for c in self.__clients:
			rlist.append(c.GetSocket())
		stime = time.time()
		etime = stime + timeout
		curtime = time.time()
		while curtime < stime or timeout == 0:
			ret = select.select(rlist,wlist,xlist,1)
			if len(ret) > 0  and len(ret[0]) > 0:
				return ret[0]
			curtime = time.time()
		return None
	def __HandleAccept(self):
		try:
			s , addr = self.__socket.accept()
		except:
			pass
		return

	def __HandleClient(self,sock):
		return

	def __HandleRead(self,rlist):
		for r in rlist:
			if r == self.__socket:
				self.__HandleAccept()
			elsif r in self.__CliSocks:
				self.__HandleClient(r)
	def __WriteCommand(self,s,cmd):
		s.WriteMessage(2,cmd)
		return
	def run(self):
		while self.__running:
			
