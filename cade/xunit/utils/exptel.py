#! python


import telnetlib
from xunit.utils import exception
import time
import select
import sys
import re

class HostRefusedError(exception.XUnitException):
	pass

class HostCmdTimeoutError(exception.XUnitException):
	pass

class ExpectTimedoutError(exception.XUnitException):
	pass



class XUnitTelnet:
	'''
	    ExpectTelnet is the class for telnet handle
	    this will handle the telnet when call user
	'''
	def __login(self):
		assert(self.__tel is None)
		try:
			self.__tel = telnetlib.Telnet(self.__host,self.__port,self.__timeout)
			if self.__usernote:
				r = self.__tel.read_until(self.__usernote,self.__timeout)
				if self.__writefh:
					self.__writefh.write(r)
				self.__tel.write(self.__user + '\n')
			if self.__passnote:
				r = self.__tel.read_until(self.__passnote,self.__timeout)
				if self.__writefh:
					self.__writefh.write(r)
				self.__tel.write(self.__pass + '\n')
			r = self.__tel.read_until(self.__cmdnote,self.__timeout)
			if self.__writefh:
				self.__writefh.write(r)
			cmd = 'export PS1=\'%s\''%(self.__ps1)
			# now to send for the ps1 to change
			self.__tel.write(cmd +'\n')
			totr = ''
			st = time.time()
			et = st + self.__timeout
			ct = st
			# this is the command ps1 to get
			vpat = re.compile('\n%s'%(self.__ps1))
			while ct < et or self.__timeout ==0 :
				# now to get the export
				rlist = [self.__tel.fileno()]
				wlist = []
				xlist = []
				ltime = et - ct
				if self.__timeout == 0:
					# if we should listen all the time ,so we should
					# give the return 1 seconds ,for key interrupt
					ltime = 1
				ret = select.select(rlist,wlist,xlist,ltime)
				if len(ret) > 0 and len(ret[0]) > 0:
					r = self.__tel.read_very_eager()
					if len(r) > 0:
						totr += r
						if self.__writefh :
							self.__writefh.write(r)
						if vpat.search(totr):
							return
				ct = time.time()
			self.__CloseTel()
			raise HostRefusedError('export ps1(%s) timeout on cmd (%s) totr (%s)'%(self.__ps1,cmd,totr))
		except:
			self.__CloseTel()
			raise HostRefusedError('can not connect %s:%d (user:%s:pass:%s)'%(self.__host,self.__port,self.__user,self.__pass))
		return
	def __init__(self,host=None,port=23,user=None,password=None,stream=sys.stdout,timeout=5,usernote='login:',passnote='assword:',cmdnote='# ',ps1='# #>>'):
		self.__host = host
		self.__port = port
		self.__user = user
		self.__pass = password
		self.__usernote = usernote
		self.__passnote = passnote
		self.__cmdnote = cmdnote
		self.__timeout = timeout
		self.__ps1 = ps1
		self.__writefh = stream
		self.__tel = None
		if self.__host is not None and self.__port is not None:
			self.__login()
		return


	def __CloseTel(self):
		if self.__tel:
			self.__tel.close()
			del self.__tel
			self.__tel = None
		return
		
	def __del__(self):
		self.__CloseTel()
		return
	def ReConnect(host=None,port=23):
		self.__CloseTel()
		if self.__host is None or self.__port is None:
			raise HostRefusedError('no host or port specify')
		self.__login()
		return
	def SetUserName(username,password=None):
		self.__user = username
		self.__pass = password
		return

	def __runcmd(self,cmd,timeout):
		totr = ''

		assert(self.__tel)
		self.__tel.write(cmd + '\n')
		# now we should find ps1 when call
		ps1pat = re.compile(self.__ps1)
		
		st = time.time()
		et = st + timeout
		ct = st
		while ct < et or timeout == 0:
			# now to get the export
			rlist = [self.__tel.fileno()]
			wlist = []
			xlist = []
			ltime = et - ct
			if timeout == 0:
				# 1 second for key interrupt
				ltime = 1
			ret = select.select(rlist,wlist,xlist,ltime)
			if len(ret) > 0 and len(ret[0]) > 0:
				r = self.__tel.read_very_eager()
				if len(r) > 0:
					totr += r
					if self.__writefh:
						self.__writefh.write(r)
					if ps1pat.search(totr):
						return totr
			ct = time.time()			
		raise HostCmdTimeoutError('export ps1(%s) timeout on cmd (%s) totr (%s)'%(self.__ps1,cmd,totr))
		return totr

	def __echo(self,on=1):
		if on :
			cmd = 'stty echo'
		else:
			cmd = 'stty -echo'
		self.__runcmd(cmd,2)
		return

	def Execute(self,cmd,timeout=5,expref=None):
		'''
			function : Execute
			execute command on the telnet
			@cmd is the command to run
			@expref 
			      regular expression
			      the expect output if  None we return 1
			        if not None we have return 1 when match 
			         0 for not match
			@timeout
			      timeout value most time we run this command
			      if timeout ,we raise HostCmdTimeoutError

			return value:
			     matchone and the string we get from the command running
		'''
		if self.__tel is None:
			raise HostRefusedError('no telnet connectted')

		# turn off the echo ,so we do not get the echo running
		self.__echo(0)
		totr = self.__runcmd(cmd,timeout)
		# turn on the echo,
		self.__echo(1)

		matched = 1
		if expref:
			vpat = re.compile(expref)
			if  not vpat.search(totr):
				matched = 0
		
		return matched,totr

	def Writeln(self,cmd):
		if self.__tel is None:
			raise HostRefusedError('no telnet connectted')

		self.__tel.write(cmd+'\n')
		return
	def Close(self):
		self.__CloseTel()
		return