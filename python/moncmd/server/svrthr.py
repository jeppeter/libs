#! python

import threading
import LocalException
import select
import time
import monproto
import logging

class MonSvrThread(threading.Thread):
	def __init__(self,port,timeout=60):
		threading.Thread.__init__(self)
		self.__timeout = timeout
		self.__port = port
		self.__running = 0
		self.__socket = None
		self.__mutex = threading.Lock()
		self.__clients = []
		self.__clientaddrs = []
		self.__reports = []
		self.__logs = []
		self.__writecmds=[]
		return 

	def __BindSocket(self):
		self.__socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		self.__socket.bind(('',self.__port))
		self.__socket.listen(5)
		self.__socket.setblocking(0)
		return 	

	def __IsWriteCmd(self):
		ret = 0
		self.__mutex.acquire()
		ret = len(self.__writecmds) > 0 and 1 or 0		
		self.__mutex.release()
		return ret
	def __GetWriteCmd(self):
		cmd = None
		self.__mutex.acquire()
		if len(self.__writecmds):
			cmd = self.__writecmds[0]
			self.__writecmds.remove(0)
		self.__mutex.release()
		return cmd
		

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
			if self.__running == 0 :
				break
			if self.__IsWriteCmd() > 0:
				break
		return None

	def __InsertClientAddr(self,sock,addr):
		self.__clients.append(sock)
		self.__clientaddrs.append(addr)
		return ret
		
		
	def __HandleAccept(self):
		try:
			s , addr = self.__socket.accept()
			sock = monproto.MonProtoSock(s)
			self.__InsertClientAddr(sock,addr)
		except:
			if sock:
				self.__clients.remove(sock)
				self.__clientaddrs.remove(addr)
			pass
		return

	def __HandleReport(addr,gtime,msg):
		ret = 1
		'''
			we should insert into the report queue
		'''
		elem = (addr,gtime,msg)
		self.__mutex.acquire()
		self.__reports.append(elem)
		self.__mutex.release()
		return ret
	def __HandleLog(msg):
		ret = 1
		'''
			we should insert the log queue
		'''
		elem = (addr,gtime,msg)
		self.__mutex.acquire()
		self.__logs.append(elem)
		self.__mutex.release()
		return ret

	def __RemoveClientAddr(self,sock):
		for i in xrange(0,len(self.__clients)):
			if self.__clients[i] == sock:
				sock.CloseSocket()
				del self.__clients[i]
				del self.__clientaddrs[i]
				return 1		
		return 0
	def __HandleClient(self,sock):
		monsock = None
		i = 0
		addr = None
		for monp in self.__clients:
			if monp.GetSocket() == sock:
				monsock = monp
				addr = self.__clientaddrs[i]
				break
			i += 1
		if monsock is None:
			return 0
		try:
			type,msg = monsock.ReadMessage()
			gtime = time.time()
			if type == monproto.REP_TYPE:
				ret = self.__HandleReport(addr,gtime,msg)
			elsif type == monproto.LOG_TYPE:
				ret = self.__HandleLog(addr,gtimemsg)
			else:
				logging.warning('type %d not recognize'%(type))
				ret = 0
		except:
			# this is error ,so we should remove the sock and address
			self.__RemoveClientAddr(monsock)
		return ret

	def __HandleRead(self,rlist):
		for r in rlist:
			if r == self.__socket:
				self.__HandleAccept()
			elsif r in self.__CliSocks:
				self.__HandleClient(r)
		return
	def __WriteCommand(self,s,cmd):
		s.WriteMessage(monproto.CMD_TYPE,cmd)
		return
	def __HandleWriteCmd(self):
		sock = None
		cmd = self.__GetWriteCmd()
		if cmd and len(cmd) == 2:
			# because we have to
			addr = cmd[0]
			msg = cmd[1]
			i = 0
			sock = None
			for i in xrange(0,len(self.__clientaddrs)):				
				if self.__clientaddrs[i] == addr:
					sock = self.__clients[i]
					break
			if sock is None:
				return 0
			self.__WriteCommand(sock,msg)
			return 1
		return 0
	def run(self):
		self.__BindSocket()
		while self.__running:
			s = self.__GetRead(self.__timeout)
			if s and len(s) > 0:
				self.__HandleRead(s)
			self.__HandleWriteCmd()
		self.__ClearRresource()
		return 0

	def InsertCmd(self,addr,msg):
		cmd = (addr,msg)
		self.__mutex.acquire()
		self.__writecmds.append(cmd)
		self.__mutex.release()
		return

	def GetLog(self):
		log = None
		self.__mutex.acquire()
		if len(self.__logs) > 0:
			log = self.__logs[0]
			self.__logs.remove(0)
		self.__mutex.release()
		return log
		
	def GetReport(self):
		msg = None
		self.__mutex.acquire()
		if len(self.__reports) > 0:
			msg = self.__reports[0]
			self.__reports.remove(0)
		self.__mutex.release()
		return msg

	def __ClearResource(self):
		self.__mutex.acquire()
		while len(self.__logs)>0:
			self.__logs.remove(0)
		while len(self.__reports) > 0:
			self.__reports.remove(0)

		while len(self.__writecmds) > 0:
			self.__writecmds.remove(0)

		self.__mutex.release()
		# now to close the socket
		assert(len(self.__clients) == len(self.__clientaddrs))
		while len(self.__clients) > 0:
			sock = self.__clients[0]
			sock.CloseSocket()
			self.__clients.remove(0)
			self.__clientaddrs.remove(0)
		assert(len(self.__clients) == 0)
		assert(len(self.__clientaddrs) == 0)
		if self.__socket:
			self.__socket.close()
			del self.__socket
		self.__socket = None
		return
	def StopThread(self):
		self.__running=0
		i = 0
		while True:
			self.join(2.0)
			if not self.isAlive():
				break
			i += 1
			assert(i <= 3)
		return
	def StartThread(self):
		self.StopThread()
		self.__running = 1
		self.start()
		return 
