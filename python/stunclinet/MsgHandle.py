'''
 Message Handle file is to make the handle message
 the Class is in the 
 we use this to make sure of the class
'''

class MessageHandleException(Exception):
	pass



class MessageHandle:
	def __init__(self):
		self._resp = ''
		self._HasProcessed = 0

	def GetResponse(self):
		return self._resp

	def __Version(self,cmds):
		self._resp = 'version 1.0'
		return
	def __HelpCmd(self,cmdname):
		
	
	def __Help(self,cmds):
		if len(cmds) > 1:
			self.__HelpCmd(cmds[1])
		else:
			for 
			

	def ProcessHandler(self,cmds=None):
		if cmds is None :
			raise MessageHandleException('cmds is None')
		if isinstance(cmds,list) is False:
			raise MessageHandleException('cmds is not list')
		if len(cmds) < 1:
			raise MessageHandleException('cmds less than 1')

		# now to make sure that the handle
		if cmds[0] == 'version':
		elif cmds[0] == 'help':
		elif cmds[0] == 'connectstun':
		elif cmds[0] == 'listenport':
		elif cmds[0] == 'sendudp':
		else:
			raise MessageHandleException('cmds[0] %s not recognize ,please use help to know'%(cmds[0]))

		return self._resp