#! python

import ConfigParser
import LocalException
import logging

class UTCfgKeyError(LocalException.LocalException):
	pass

class UTCfgOverflowError(LocalException.LocalException):
	pass

class UTConfig:
	def __init__(self,fname=None):
		self.__IncludeFiles = []
		self.__FuncLevel = 0
		self.__MainCfg = None
		if fname :
			self.__LoadFile(fname)
			self.__MainName = fname
		else:
			self.__MainName = None

	def __del__(self):
		self.__MainName = None
		self.__IncludeFiles = []
		assert( self.__FuncLevel == 0)


	def __AddOption(self,m,p):
		for s in p.sections():
			for c in p.options(s):
				if m.has_section(s):
					if m.has_option(s,c):
						logging.warning('redefined [%s].%s'%(s,c))
						m.set(s,c,p.get(s,c))
					else:
						m.set(s,c,p.get(s,c))
				else:
					m.add_section(s)
					m.set(s,c,p.get(s,c))
		return m

	def __ParseIncludeFiles(self,cfg):
		pass

	def __LoadFile(self,fname):
		'''
			this is to load the files to the config
			and we should test for the file
			most level to be 30
		'''
		if self.__FuncLevel >= 30:
			raise UTCfgOverflowError('Load %s fname overflow'%(fname))
		if fname in self.__IncludeFiles:
			# we have already include this file
			return
		# we parse the file
		try:
			cfg = ConfigParser.RawConfigParser()
			cfg.read(fname)
		except:
			raise LocalException.LocalException('can not parse file %s'%(fname))
		# now to add the option
		if self.__MainCfg is None:
			self.__MainCfg = ConfigParser.RawConfigParser()
		self.__IncludeFiles.append(fname)

		############################
		#load special config 
		# include_files
		# unit_test_modules
		############################
		self.__AddOption(self.__MainCfg,cfg)
		return 
			

	def __GetValue(self,section,item,expand=1,level=0):
		'''
			the  section name ,and expand will expand the value of 
			expand == 1 : will expand the value '%(value)s' to the real value
			level will be 
		'''
		pass

	
