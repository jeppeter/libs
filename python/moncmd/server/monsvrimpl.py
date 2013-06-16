#! python

import svrthr

class MonSvrImpl:
	def __init__(self,port,timeout=60):
		self.__port = port
		self.__timeout = timeout
		self.__thread = None
		return
	def Start(self):
		self.Stop()
		return
	def Stop(self):
		if self.__thread :
			self.__thread.
			
		self.__thread = None
		return
	def __del__(self):
		self.Stop()
		return
