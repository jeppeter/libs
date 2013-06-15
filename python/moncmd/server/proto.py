#! python

'''
	protocol 
	length |  type | message
	2 bytes | 2 bytes | message
'''

import socket
import LocalException
class MonRecvError(LocalException.LocalException):
	pass
class MonProtoSock:
	def __init__(self,sock):
		self.__sock = sock
		return

	def GetSocket(self):
		return self.__sock
	def ReadMessage(self):
		assert(self.__sock)
		buf = self.__sock.recv(8192)
		if len(buf) < 2:
			raise MonRecvError('receive error')
		size = struct.unpack('<H',buf[:2])
		while len(buf) < (size + 2):
			buf += self.__sock.recv(8192)
		type,msg = struct.unpack('<HS*',buf[2:4],buf[4:])
		return type,msg

	def WriteMessage(self,type,msg):
