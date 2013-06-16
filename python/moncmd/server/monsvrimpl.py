#! python

import svrthr
import time
import LocalException

class SvrExitError(LocalException.LocalException):
	pass

class SvrTimeOut(LocalException.LocalException):
	pass

class MonSvrImpl:
	def __init__(self,port,timeout=60):
		self.__port = port
		self.__timeout = timeout
		self.__expiretime = timeout * 2
		self.__thread = None
		self.__reports = []
		return
	def Start(self):
		self.Stop()
		try:
			assert(self.__thread is None)
			self.__thread = svrthr.MonSvrThread(self.__port,self.__timeout)
			self.__thread.StartThread()
		except :
			raise LocalException.LocalException('can not bind on %d '%(self.__port))
		return
	def Stop(self):
		if self.__thread :
			self.__thread.StopThread()
			del self.__thread
		self.__thread = None
		return
	def __del__(self):
		self.Stop()		
		return

	def __InsertReport(self,rep):
		for i in xrange(0,len(self.__reports)):
			r = self.__reports[i]
			if r[0] == rep[0]:
				# it is the same ,so we should replace
				self.__reports[i]=rep
				return
		# not in it ,so we should append to the end
		self.__reports.append(rep)
		return
	def __DeleteTimeoutReports(self):
		curt = time.time()
		for i in xrange(0,len(self.__reports)):
			r = self.__reports[i]
			exptime = r[1] + self.__expiretime
			if exptime < curt:
				# it is timeout ,so delete it
				del self.__reports[i]
		return
	
	def __InsertReports(self,reports):
		for rep in reports:
			self.__InsertReport(rep)
		self.__DeleteTimeoutReports()
		return
	
	def GetReportResult(self,addr=None):
		# now first to make sure that the result is ok
		reports = []
		rep = None
		assert(self.__thread);
		if not self.__thread.isAlive():
			raise SvrExitError('thread exited')
		while True :
			rep = self.__thread.GetReport()
			if rep is None:
				break
			reports.append(rep)
		self.__InsertReports(reports)
		# just return reports
		if addr is None:
			return self.__reports
		reports = []
		for s in self.__reports:
			if addr == s[0]:
				reports.append(s)
		return reports

	def __GetLog(self,addr,timeout=0):
		assert(self.__thread);
		if not self.__thread.isAlive():
			raise SvrExitError('thread exited')
		i = 0
		exptime = timeout* 10
		while True:
			log = self.__thread.GetLog()
			if log and log[0] == addr:
				break
			time.sleep(0.1)
			i += 1
			if i >= exptime and timeout != 0:
				raise SvrTimeOut('Get Log Return %d'%(timeout))
		return log

	def WriteCmd(self,addr,cmd,timeout=0):
		assert(self.__thread);
		if not self.__thread.isAlive():
			raise SvrExitError('thread exited')
		self.__thread.InsertCmd(addr,cmd)
		return self.__GetLog(addr,timeout)