#! python

import sys

class AClass:
	def __init__(self):
		self.text = 'textA'
	def CallFuncA(self,msg):
		print ('AClass:%s::CallFuncA (%s)'%(self.text,msg))

	def CallFuncB(self):
		print ('AClass::CallFuncB')

	def RetA(self,little='ok'):
		print ('AClass::RetA (%s)'%(little))
		return 'AClass %s'%little

	def RetTwo(self):
		print('AClass::RetTwo')
		return 'AClass','RetTwo'

	def KwargsFunc(self,**kwargs):
		s = 'AClass\n'
		for k in kwargs.keys():
			s += '[%s]=%s\n'%(k,kwargs[k])
		print(s)
		return
	def AllArgsFunc(self,*args,**kwargs):
		s = 'AClass\n'
		for k in args:
			s += '%s'%(str(k))
			s += '\n' 
		for k in kwargs.keys():
			s += '[%s]=%s\n'%(k,kwargs[k])
		print(s)
		return
		

class BClass:
	def __init__(self):
		self.text = 'textB'
	def CallFuncA(self,msg):
		print ('BClass:%s::CallFuncA (%s)'%(self.text,msg))

	def CallFuncB(self):
		print ('BClass::CallFuncB')

	def RetA(self,little='ok'):
		print ('BClass::RetA (%s)'%(little))
		return 'BClass %s'%little

	def RetTwo(self):
		print('BClass::RetTwo')
		return 'BClass','RetTwo'

	def KwargsFunc(self,**kwargs):
		s = 'BClass\n'
		for k in kwargs.keys():
			s += '[%s]=%s\n'%(k,kwargs[k])
		print(s)
		return
	def AllArgsFunc(self,*args,**kwargs):
		s = 'BClass\n'
		for k in args:
			s += str(k)
			s += '\n' 
		for k in kwargs.keys():
			s += '[%s]=%s\n'%(k,kwargs[k])
		print(s)
		return

class ComBind:
	def __appendClass(self,cls):
		mm = __import__(self.__module__)
		if not hasattr(mm,cls):
			raise Exception('can not get %s'%(cls))
		_cls = getattr(mm,cls)
		self.__loggers.append(_cls())
		return
	def __init__(self,*cls):
		self.__loggers = []
		for clsn in cls:
			self.__appendClass(clsn)
		return
	def __getattr__(self, name):
		if hasattr(self,name):
			return self.__dict__[name]

		def _missing(*args,**kwargs):
			ret =''
			for c in self.__loggers:
				_f = getattr(c,name)
				if len(args) > 0 and len(kwargs.keys())>0:
					ret=_f(*args,**kwargs)
				elif len(args) > 0:
					ret=_f(*args)
				elif len(kwargs.keys()) >0:
					ret=_f(**kwargs)
				else:
					ret=_f()
			return ret
		return _missing
		

c = ComBind('BClass','AClass')



r = c.CallFuncA('hello')
print(r)
c.CallFuncB()
r = c.RetA('go')
print(r)
r = c.RetTwo()
print(r)
c.KwargsFunc(hello='no',new='world')

c.AllArgsFunc('go','yes',man='2man',woman='3child')