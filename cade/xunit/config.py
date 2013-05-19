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
	TestBase=10
	UnitTest=20

	the 10 and 20 is the sequence number the smallest is the for the 
	
'''
import xunit.utils.XConfigParser
from xunit.utils import exception
import logging
import sys
import re
import os

class XUnitConfigKeyError(exception.XUnitException):
	pass

class XUnitConfigOverflowError(exception.XUnitException):
	pass
class XUnitConfigLoadFileError(exception.XUnitException):
	pass

class XUnitConfigBase:
	def __ResetCfg(self):
		if hasattr(self,'__MainCfg') and self.__MainCfg:
			del self.__MainCfg
		self.__MainCfg = None
		self.__IncludeFiles = []
		self.__UnitTests = []
		self.__MainName = None
		self.__SearchPaths = []
		self.__FuncLevel = 0
		self.__ConfigFile=''
		return 

	def __init__(self,fname=None):
		self.__ResetCfg()
		if fname :
			self.__LoadFile(fname)
			assert(self.__FuncLevel == 0)
			self.__MainName = fname
			self.__ExpandAllKeys()
			self.__ConfigFile = fname
		else:
			self.__MainName = None

	def __del__(self):
		self.__ResetCfg()
		assert( self.__FuncLevel == 0)

	def __AddOption(self,m,p,override=0):
		for s in p.sections():
			for c in p.options(s):
				if m.has_section(s):
					if m.has_option(s,c) and override == 0:
						logging.warning('redefined [%s].%s'%(s,c))
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
				if c not in self.__SearchPaths and v !='n':
					path = self.__ExpandKey(s,c)
					self.__SearchPaths.append(path)
					if path not in sys.path:	
						sys.path.append(path)
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
			raise XUnitConfigOverflowError('Load %s fname overflow'%(fname))
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
				for path in sys.path:
					if os.path.isfile(path+os.sep+fname):
						filefind = path + os.sep+fname
						break
				if filefind is None:
					if os.path.isfile('.'+os.sep+fname):
						filefind = '.'+os.sep+fname
					else:
						raise XUnitConfigLoadFileError('could not find file %s in %s'%(fname,self.__SearchPaths))
			cfg = xunit.utils.XConfigParser.ConfigParser()
			cfg.read(filefind)
		except:
			raise XUnitConfigLoadFileError('can not parse file %s'%(fname))
		# now to add the option
		if self.__MainCfg is None:
			self.__MainCfg = xunit.utils.XConfigParser.ConfigParser()
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
		
		return 

	def __SplitKey(self,k):
		r = k.find('.')
		s = k
		o = None
		if r >= 0:
			kpart = k
			while len(kpart) > 0:
				r = kpart.rfind('.')
				if r < 0 :
					break
				s = k[:r]
				o = k[r+1:]
				# if it is end of '.' so we should make this ok
				if self.__MainCfg.has_option(s,o):
					return s,o
				kpart = s

			kpart = k
			while len(kpart) > 0:
				r = kpart.rfind('.')
				if r < 0:
					break
				s = k[:r]
				o = k[r+1:]
				if self.__MainCfg.has_section(s):
					return s,o
				kpart = s

			# now we should get the last
			kpart = k
			r = kpart.rfind('.')
			s = k[:r]
			o = k[r+1:]
		return s ,o

	def __ExpandValue(self,section,option,k,values=None):
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		try:
			# we try to expand the key value from "\x20\x33" to " 3"
			# if we have already in " 3" so we just make
			v = eval(k)
		except :
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
 								raise XUnitConfigOverflowError('expand value %s overflow '%(k))
							v = self.__ExpandValue(sec,opt,v,values)
							try:
								values[s] = eval(v)
							except :
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
								raise XUnitConfigOverflowError('expand value %s overflow '%(k))
							v = self.__MainCfg.get(section,sec,1)
							v = self.__ExpandValue(section,sec,v,values)
							try:
								values[s] = eval(v)
							except :
								values[s] = v
						finally:
							self.__FuncLevel -= 1
		# we will expand for the value				
		tmpv = self.__MainCfg.get(section,option,0,values)		
		try:
			# we try to expand the key value from "\x20\x33" to " 3"
			# if we have already in " 3" so we just make
			v = eval(tmpv)
		except :
			v = tmpv		
		return v

	def __GetValue(self,section,item,defval='',expand=1,valuemap={}):
		'''
			the  section name ,and expand will expand the value of 
			expand == 1 : will expand the value '%(value)s' to the real value
			level will be expand to it
		'''
		if self.__MainCfg is None:
			return defval
		v = defval
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
	def __ExpandAllKeys(self):
		'''
			if the keys to be regular string not including the %(section.option)s value replace
		'''
		if self.__MainCfg is None:
			return
		p = '%\(([^)]+)\)s'
		vpat = re.compile(p)
		findone = 1
		while findone > 0:
			findone = 0
			for s in self.__MainCfg.sections():
				for o in self.__MainCfg.options(s):
					if vpat.search(o):
						v = self.__MainCfg.get(s,o,1)
						eo = self.__ExpandKey(s,o)
						if self.__MainCfg.has_option(s,eo):
							logging.warning('[%s].%s=%s'%(s,eo,self.__MainCfg.get(s,eo,1)))
						self.__MainCfg.set(s,eo,v)
						self.__MainCfg.remove_option(s,o)
						# we rescan the options and sections
						findone = 1
		return

	def GetValue(self,sec,opt,defval='',expand=1):
		return self.__GetValue(sec,opt,defval,expand)

	def LoadFile(self,fname):
		assert(self.__FuncLevel == 0)
		self.__LoadFile(fname)
		if self.__MainName is None:
			self.__MainName = fname
		# expand all keys
		self.__ExpandAllKeys()
		assert(self.__FuncLevel == 0)
		self.__ConfigFile = fname
		return 

	def GetIncludeFiles(self):
		return self.__IncludeFiles

		
	def GetSearchPaths(self):
		return self.__SearchPaths

	def SetValue(self,section,option,value,override=0):
		if self.__MainCfg is None:
			self.__MainCfg = xunit.utils.XConfigParser.ConfigParser()
		cfg = xunit.utils.XConfigParser.ConfigParser()
		cfg.add_section(section)
		cfg.set(section,option,value)
		self.__AddOption(self.__MainCfg,cfg,override)
		############################
		#load special config 
		# [.path]
		# [.include]
		# [.unit.test]
		# sections to be the value
		############################
		self.__AddSearchPathSection(cfg)
		self.__AddIncludeFiles(cfg)
		return

	def AddSearchPath(self,path):
		return self.SetValue('.path',path,'y',1)

	def GetSections(self,sec):
		values={}
		if self.__MainCfg and self.__MainCfg.has_section(sec):
			for o in self.__MainCfg.options(sec):
				v = self.GetValue(sec,o)
				values[o] = v
		return values
	def GetUnitTests(self):
		units = []
		values = self.GetSections('.unit.test')
		sortv = {}
		try:
			for k in values.keys():
				if values[k] != 'n':
					sortv[int(values[k])]=k
		except:
			raise XUnitConfigKeyError('value of k %s in %s'%(k,values[k]))
		
		for k in sorted(sortv.keys()):
			units.append(sortv[k])		
		return units

	def GetSectionsPattern(self,pat=None):
		vpat = None
		if pat is not None:
			vpat = re.compile(pat)
		rs = [] 

		if self.__MainCfg :
			for s in self.__MainCfg.sections():
				if vpat is None or vpat.search(s):
					rs.append(s)
		return rs

	def GetOptionsPattern(self,sec,pat=None):
		vpat = None
		if pat is not  None:
			vpat = re.compile(pat)
		ro = []
		if self.__MainCfg :
			for o in self.__MainCfg.options(sec):
				if vpat is None or vpat.search(o):
					ro.append(o)
		return ro
	def GetConfigFile(self):
		return self.__ConfigFile
		
def singleton(cls):
	instances = {}
	def get_instance():
		if cls not in instances:
			instances[cls] = cls()
		return instances[cls]
	return get_instance

@singleton
class XUnitConfig(XUnitConfigBase):
	pass
