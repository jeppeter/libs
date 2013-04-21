#! python


'''
	this file for the ut config
	and it will expand the value by the 
	special sections are 
	.include
	.path
	.unit.test

	[.include]
	base.cfg=y
	inc.cfg=y

	[.path]
	/usr/inc=y


	[.unit.test]
	TestBase=y
	UnitTest=y
	
'''
import ConfigParser
import LocalException
import logging
import sys
import re
import os

class UTCfgKeyError(LocalException.LocalException):
	pass

class UTCfgOverflowError(LocalException.LocalException):
	pass
class UTCfgLoadFileError(LocalException.LocalException):
	pass

class UTConfigBase:
	def __ResetCfg(self):
		if hasattr(self,'__MainCfg') and self.__MainCfg:
			del self.__MainCfg
		self.__MainCfg = None
		self.__IncludeFiles = []
		self.__UnitTests = []
		self.__MainName = None
		self.__SearchPaths = []
		self.__FuncLevel = 0
		return 

	def __init__(self,fname=None):
		self.__ResetCfg()
		if fname :
			self.__LoadFile(fname)
			assert(self.__FuncLevel == 0)
			self.__MainName = fname
		else:
			self.__MainName = None

	def __del__(self):
		self.__ResetCfg()
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
				#logging.info('[%s].%s=%s'%(s,c,p.get(s,c,1)))
				#logging.info('main.[%s].%s=%s'%(s,c,m.get(s,c,1)))
		return m


	def __AddSearchPathSection(self,cfg):
		s = '.path'
		if cfg.has_section(s):
			# now to add the search path
			for c in cfg.options(s):
				v = cfg.get(s,c)
				if c not in self.__SearchPaths and v =='y':
					path = self.__ExpandKey(s,c)
					self.__SearchPaths.append(path)
					sys.path.append(path)
		return

	def __AddUnitTestSection(self,cfg):
		s = '.unit.test'
		if cfg.has_section(s):
			for c in cfg.options(s):
				v = cfg.get(s,c)
				if v == 'y' and v not in self.__UnitTests:
					self.__UnitTests.append(c)
		return
					

	def __AddIncludeFiles(self,cfg):
		# now we should plus the func level for including
		s = '.include'
		if cfg.has_section(s):
			for c in cfg.options(s):
				v = cfg.get(s,c,1)				
				if v == 'y' :
					fname = self.__ExpandKey(s,c)
					try:
						self.__FuncLevel += 1
						self.__LoadFile(fname)
					finally:
						self.__FuncLevel -= 1
		return

	def __ReplaceValue(self,k,values):
		v = k
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		if vpat.search(k):
			sarr = re.findall(p,k)
			for s in sarr:
				assert(s in values.keys())
				v = v.replace('%%(%s)s'%(s),values[s])
		return v
	def __ExpandKey(self,section,k,values={}):
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		v = k
		if vpat.search(k):
			sarr = re.findall(p,k)
			assert(len(sarr) > 0)
			assert(self.__MainCfg)
			for s in sarr:
				sec,opt = self.__SplitKey(s)
				if opt:
					if self.__MainCfg.has_option(sec,opt):
						v = self.__MainCfg.get(sec,opt,1)
						# if we have expand the key
						v = self.__ExpandValue(sec,opt,s,values)
						values[s]=v
					else:
						values[s]=''
				else:
					if self.__MainCfg.has_option(section,sec):
						v = self.__MainCfg.get(section,sec,1)
						v = self.__ExpandValue(section,sec,v,values)
						values[s]=v
					else:
						values[s]=''
			# it is to expand the value
			v = self.__ReplaceValue(k,values)
		return v

	def __DebugCfg(self,cfg):
		for s in cfg.sections():
			for o in cfg.options(s):
				logging.info('[%s].%s=%s'%(s,o,cfg.get(s,o,1)))
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
		# now to test if the file is ex
		try:
			# now to test for the file
			filefind = None
			for path in self.__SearchPaths:
				if os.path.isfile(path+os.sep+fname):
					filefind = path + os.sep+fname
					break
			if filefind is None:
				if os.path.isfile(fname):
					filefind = fname					
				else:
					raise UTCfgLoadFileError('could not find file %s in %s'%(fname,self.__SearchPaths))
			cfg = ConfigParser.ConfigParser()
			cfg.read(filefind)
		except:
			raise UTCfgLoadFileError('can not parse file %s'%(fname))
		# now to add the option
		if self.__MainCfg is None:
			self.__MainCfg = ConfigParser.ConfigParser()
		self.__IncludeFiles.append(fname)

		# we have to add option first ,because when call the 
		# add includefiles or searchpathsections ,to make it ok
		self.__AddOption(self.__MainCfg,cfg)
		############################
		#load special config 
		# [.path]
		# [.include]
		# [.unit.test]
		# sections to be the value
		############################
		self.__AddSearchPathSection(cfg)
		self.__AddIncludeFiles(cfg)
		self.__AddUnitTestSection(cfg)
		
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

	def __ExpandValue(self,section,option,k,values=None):
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		v = k
		if values is None:
			values = {}
		if vpat.search(k):
			# now to make sure for the 
			sarr = re.findall(p,k)
			assert(len(sarr) > 0)
			assert(self.__MainCfg)
			for s in sarr:
				# now we test for it 
				sec,opt = self.__SplitKey(s)
				if opt:
					# if we have find the section and option
					if self.__MainCfg.has_section(sec) and self.__MainCfg.has_option(sec,opt):
						v = self.__MainCfg.get(sec,opt,1)
						try:
							self.__FuncLevel += 1
							if self.__FuncLevel >= 30:
								raise UTCfgOverflowError('expand value %s overflow '%(k))
							v = self.__ExpandValue(sec,opt,v,values)
							values[s] = v
							break
						finally:
							self.__FuncLevel -= 1
					else:
						values[s] = ''

				else:
					if self.__MainCfg.has_option(section,sec) :
						try:
							self.__FuncLevel += 1
							if self.__FuncLevel >= 30:
								raise UTCfgOverflowError('expand value %s overflow '%(k))
							v = self.__MainCfg.get(section,sec,1)
							v = self.__ExpandValue(section,sec,v,values)
							values[s] = v
						finally:
							self.__FuncLevel -= 1
		# we will expand for the value				
		v = self.__MainCfg.get(section,option,0,values)
		return v

	def __GetValue(self,section,item,expand=1,valuemap={}):
		'''
			the  section name ,and expand will expand the value of 
			expand == 1 : will expand the value '%(value)s' to the real value
			level will be expand to it
		'''
		if self.__MainCfg is None:
			return ''
		v = ''
		if self.__MainCfg.has_section(section):
			if self.__MainCfg.has_option(section,item):
				# now we should give the value expand
				if expand :
					# now expand ,so we should expand value
					tmpv = self.__MainCfg.get(section,item,1)
					v = self.__ExpandValue(section,item,tmpv,valuemap)
				else:
					# now expand ,so we get the raw value
					v = self.__MainCfg.get(section,item,1)
		return v

	def GetValue(self,sec,opt,expand=1):
		return self.__GetValue(sec,opt,expand)

	def LoadFile(self,fname):
		self.__LoadFile(fname)
		if self.__MainName is None:
			self.__MainName = fname
		return 

	def GetIncludeFiles(self):
		return self.__IncludeFiles

	def GetUnitTests(self):
		return self.__UnitTests
		
	def GetSearchPaths(self):
		return self.__SearchPaths

	def SetValue(self,section,option,value,force=0):
		if self.__MainCfg is None:
			self.__MainCfg = ConfigParser.ConfigParser()
		if self.__MainCfg.has_option(section,option) and force == 0:
			raise UTCfgKeyError('[%s].%s has value %s reset it'%(section,option,self.__MainCfg.get(section,option,1)))
		if not self.__MainCfg.has_section(section):
			self.__MainCfg.add_section(section)
		self.__MainCfg.set(section,option,value)
		return

class SingletonDecorator(object):
    def __init__(self, cls):
        self._cls = cls
        self._inst = None
    def __call__(self, *args, **kwargs):
        ''' Over __call__ method. So the instance of this class
        can be called as a function. '''
        if not self._inst:
            self._inst = self._cls(*args, **kwargs)
        return self._inst

UTConfig=SingletonDecorator(UTConfigBase)