#! python

import ConfigParser
import LocalException
import logging
import sys
import re

class UTCfgKeyError(LocalException.LocalException):
	pass

class UTCfgOverflowError(LocalException.LocalException):
	pass

class UTConfig:
	def __init__(self,fname=None):
		self.__IncludeFiles = []
		self.__SearchPaths=[]
		self.__UnitTests = []
		self.__FuncLevel = 0
		self.__MainCfg = None
		if fname :
			self.__LoadFile(fname)
			assert(self.__FuncLevel == 0)
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
						m.set(s,c,p.get(s,c,1))
					else:
						m.set(s,c,p.get(s,c,1))
				else:
					m.add_section(s)
					m.set(s,c,p.get(s,c,1))
		return m


	def __AddSearchPathSection(self,cfg):
		s = 'search_path'
		if cfg.has_section(s):
			# now to add the search path
			for c in cfg.options(s):
				v = cfg.get(s,c)
				if c not in self.__SearchPaths:
					self.__SearchPaths.append(c)
					sys.path.append(c)
		return

	def __AddUnitTestSection(self,cfg):
		s = 'unit_test'
		if cfg.has_section(s):
			for c in cfg.options(s):
				v = cfg.get(s,c)
				if v == 'y' and v not in self.__UnitTests:
					self.__UnitTests.append(c)
		return
					

	def __AddIncludeFiles(self,cfg):
		# now we should plus the func level for including
		s = 'include_files'
		if cfg.has_section(s):
			for c in cfg.options(s):
				v = cfg.get(s,c)
				if v == 'y' :
					try:
						self.__FuncLevel += 1
						sefl.__LoadFile(c)
					finally:
						self.__FuncLevel -= 1
		return

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
			cfg = ConfigParser.ConfigParser()
			cfg.read(fname)
		except:
			raise LocalException.LocalException('can not parse file %s'%(fname))
		# now to add the option
		if self.__MainCfg is None:
			self.__MainCfg = ConfigParser.ConfigParser()
		self.__IncludeFiles.append(fname)

		############################
		#load special config 
		# [search_path]
		# [include_files]
		# [unit_test]
		# sections to be the value
		############################
		self.__AddSearchPathSection(cfg)
		self.__AddIncludeFiles(cfg)
		self.__AddUnitTestSection(cfg)
		
		self.__AddOption(self.__MainCfg,cfg)
		return 

	def __SplitKey(self,k):
		r = k.find('.')
		s = k
		v = None
		if r >= 0:
			r = k.rfind('.')
			s = k[:r]
			v = k[r+1:]
			# if it is end of '.' so we should make this ok
			if len(v) == 0:
				s = k
				v = None
		return s ,v

	def __ExpandValue(self,k):
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		v = k
		if vpat.search(k):
			# now to make sure for the 
			sarr = re.findall(p,k)
			assert(len(sarr) > 0)
			assert(self.__MainCfg)
			for s in sarr:
				# now we test for it 
				sec,opt = self.__SplitKey(s)
				if opt:

				else:
				
		return v

	def __GetValue(self,section,item,expand=1,valuemap={}):
		'''
			the  section name ,and expand will expand the value of 
			expand == 1 : will expand the value '%(value)s' to the real value
			level will be expand to it
		'''
		if self.__MainCfg is None:
			return None
		v = None
		if self.__MainCfg.has_section(section):
			if self.__MainCfg.has_option(section,item):
				# now we should give the value expand
				if expand :
					# now expand ,so we should expand value
					tmpv = self.__MainCfg.get(section,item,1)
				else:
					# now expand ,so we get the raw value
					v = self.__MainCfg.get(section,item,1)
		return v
