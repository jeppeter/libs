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
		self.__CmdHelpDict = {
		'version' : 'version 1.0',
		'connectstun' : 'connect [stunserver] local_bind_port' ,
		'sendudp' : 'send local_bind_port remote_ip remote_port [send_message] default message is "openvpn verify message"',
		'startcmd' : 'start args [...] run the command',
		'localudpproxy' : 'proxy local_listen_port [local_ip] local_forward_port ; in the middle of the proxy of udp',
		'help' : 'to display total cmd help message',
		}
		self.__CmdFuncTable ={
			'version'         : MessageHandle.__Version,
			'connectstun'     : MessageHandle.__ConnectStun,
			'sendudp'         : MessageHandle.__SendUdp,
			'startcmd'        : MessageHandle.__StartCmd,
			'localudpproxy'   : MessageHandle.__LocalUdpProxy,
			'help'            : MessageHandle.__Help
		}

	def __Version(self,args):
		pass
	def __ConnectStun(self,args):
		pass
	def __SendUdp(self,args):
		pass
	def __StartCmd(self,args):
		pass
	def __LocalUdpProxy(self,args):
		pass
	def __Help(self,args):
		if args is None:
			for a in self.__CmdHelpDict.keys():
				if a != 'help':
					self._resp += self.__CmdHelpDict[a]
					self._resp += '\n'
		else:
			for a in args:
				if a in self.__CmdHelpDict.keys():
					self._resp += self.__CmdHelpDict[a]
					self._resp += '\n'
				else:
					raise MessageHandleException('unkown %s please use help for more information'%(repr(a)))
		return self._resp


	def GetResponse(self):
		return self._resp

			

	def ProcessHandler(self,cmds=None):
		if cmds is None :
			raise MessageHandleException('cmds is None')
		if isinstance(cmds,list) is False:
			raise MessageHandleException('cmds is not list')
		if len(cmds) < 1:
			raise MessageHandleException('cmds less than 1')

		# now to make sure that the handle
		if cmds[0] in self.__CmdFuncTable.keys():
			self.__CmdFuncTable[cmds[0]](self,cmds[1:])
		else:
			raise MessageHandleException('cmds[0] %s not recognize ,please use help to know'%(cmds[0]))

		return self._resp
