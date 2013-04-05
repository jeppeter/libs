#! python

import ConfigParser
import LocalException

class UTCfgKeyError(LocalException.LocalException):
	pass

class UTCfgOverflowError(LocalException.LocalException):
	pass

class UTConfig:
	def __init__(self,fname=None):
		self.__IncludeFiles = []
		if fname :
			self.__MainName = fname
			self.__IncludeFiles.append(fname)
		else:
			self.__MainName = None
		self.__FuncLevel = 0

	def __del__(self):
		self.__MainName = None
		self.__IncludeFiles = []
		assert( self.__FuncLevel == 0)

	def __LoadFile(self,fname):
		'''
			this is to load the files to the config
			and we should test for the file
			most level to be 30
		'''
		if self.__FuncLevel >= 30:
			raise UTCfgOverflowError('Load %s fname overflow'%(fname))
		

	def __GetValue(self,section,item,expand=1,level=0):
		'''
			the  section name ,and expand will expand the value of 
			expand == 1 : will expand the value '%(value)s' to the real value
			level will be 
		'''
		pass

	
