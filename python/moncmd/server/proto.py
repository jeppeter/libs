#! python

'''
	protocol 
	length |  type | message
	2 bytes | 2 bytes | message
'''

import socket
class MonProtoSock:
	def __init__(self,sock):
		self.__sock = sock
		return

	def GetSocket(self):
		return self.__sock
	def ReadMessage(self):

	def WriteMessage(self,type,msg):
