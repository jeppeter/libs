'''
 Message Handle file is to make the handle message
 the Class is in the 
 we use this to make sure of the class
'''

class MessageHandleException(Exception):
	pass

__CmdHelpDict = {
	'version' : 'version 1.0',
	'connectstun' : 'connect [stunserver] local_bind_port' ,
	'sendudp' : 'send local_bind_port remote_ip remote_port [send_message] default message is "openvpn verify message"',
	'startcmd' : 'start args [...] run the command',
	'localudpproxy' : 'proxy local_listen_port [local_ip] local_forward_port ; in the middle of the proxy of udp',
	'help' : 'to display total cmd help message',
}

class MessageHandle:
	def __init__(self):
		self._resp = ''
		self._HasProcessed = 0

	def GetResponse(self):
		return self._resp

	def __HelpCmd(self,cmdname):
		if cmdname in __CmdHelpDict.keys():
			self._resp += __CmdHelpDict[cmdname]
		else:
			raise MessageHandleException('unknown command name %s'%(cmdname))
		return self._resp
	
	def __Help(self,cmds):
		if 'help' in cmds:
			for c in __CmdHelpDict.keys():
				if c != 'help':
					self.__HelpCmd(c)
		else:
			return self.__HelpCmd(cmds[1])
			

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