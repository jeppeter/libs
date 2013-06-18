
import threading
import LocalException
import time

class CliInvalidError(LocalException.LocalException):
	pass

class RunCmdThread(threading.Thread):
	def __init__(self,cmd,timeout=6):
		self.__running = 0
		self.__cmd = cmd
		self.__timeout = timeout
		self.__pipe=None
		self.__errres = None
		self.__outres = None
		return

	def __IsExited(self):
		ret = 1
		if self.__pipe:
			if self.__pipe.poll():
				ret = 0
		return ret

	def __WaitAndKillProcess(self,timeout=0):
		if self.__pipe:
			times = timeout * 10
			i = 0
			while self.__running:
				if self.__IsExited():
					break
				time.sleep(0.1)
				i += 1
				try:
					rl = self.__pipe.stdout.readline()
					self.__outres.append(rl)
				except:
					pass
				try:
					rl = self.__pipe.stderr.readline()
					self.__errres.append(rl)
				except:
					pass
				if times != 0 and i >= times/2:
					self.__pipe.send_signal(2)
				if times != 0 and i >= times:
					self.__pipe.send_signal(9)
			while True:
				if  self.__IsExited():
					break
				self.__pipe.send_signal(9)
				time.sleep(0.1)
			try:
				rl = self.__pipe.stdout.readlines()
				self.__outres.extend(rl)
			except:
				pass
			try:
				rl = self.__pipe.stderr.readlines()
				self.__errres.extend(rl)
			except:
				pass
		return

	def __KillProcess(self):
		if self.__pipe:
			while True:
				if not self.__pipe.isAlive():
					break
				self.__pipe.send_signal(9)
				time.sleep(0.1)
			del self.__pipe
		self.__pipe = None
		return
			
	def __RunCmd(self):
		assert(self.__pipe is None)
		self.__outres = []
		self.__errres =  []
		try:
			self.__pipe = subprocess.Popen(self.__cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
		except:
			self.__KillProcess()
			return -1
		return 0
	def run(self):
		# now to start command
		ret = self.__RunCmd()
		if ret < 0:
			return ret
		self.__WaitAndKillProcess(self.__timeout)
		return 0
			
	
	def StopThread(self):
		self.__running = 0
		while True:
			if not self.isAlive():
				break
			self.join(0.1)	
		self.__pipe = None
		return
	def StartThread(self):
		self.StopThread()
		self.__errres = []
		self.__outres = []
		assert(self.__pipe is None)
		# now to start process
		self.start()
		return 0

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
		if self.__sock:
			self.__sock.CloseSocket()
			del self.__sock
		self.__sock = None

	def __SetNonBlock(self,sock):
		flags = fcntl.fcntl(sock.fileno(),F_GETFL)
		fcntl.fcntl(sock.fileno(),F_SETFL,flags | O_NONBLOCK)
		return
	def __Connect(self):
		asssert(self.__sock is None)
		sock = None
		try:
			self.__sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			self.__sock.connect((self.__host,self.__port))
			self.__SetNonBlock(self,self.__sock)
		except:
			if sock :
				sock.close()
				del sock
			sock = None
			raise 
		return

	def StartThread(self):
		self.StopThread()
		self.start()
		return 0

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
		try:

		except:
			self.__ClearResource()
			return -3

