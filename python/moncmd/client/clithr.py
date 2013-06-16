
import threading
import LocalException

class CliInvalidError(LocalException.LocalException):
	pass

class RunCmdThread(threading.Thread):
	def __init__(self,cmd,timeout=6):
		self.__running = 0
		self.__cmd = cmd
		self.__timeout = timeout
		self.__pipe=None
		return
	def StopThread(self):
		if self.__pipe :
	def StartThread(self):
		self.StopThread()

class MonCliThread(threading.Thread):
	def __init__(self,hostport,timeout=60):
		threading.Thread.__init__(self)
		self.__timeout = timeout
		arr = hostport.split(':')
		if len(arr) <= 1:
			raise CliInvalidError('not valid (%s) hostport'%(hostport))
		try:
			self.__host = arr[0]
			self.__port = int(arr[1])
		except:
			raise CliInvalidError('not valid (%s) hostport'%(hostport))
		self.__sock = None
		self.__running = 0

	def __ClearResource(self):
		if self.__

	def StartThread(self):
		pass

	def StopThread(self,timeout= 6):		
		if self.__running :
			times = timeout * 10
			i = 0
			while self.isAlive():
				self.__running = 0
				self.join(0.1)
				i += 1
				assert(i <= times or timeout == 0)
			assert(self.__sock is None)	

	def __RunCmd(self,cmd):
		pass
	def __SendReport(self):
		assert(self.__sock)

	def run(self):
		pass

