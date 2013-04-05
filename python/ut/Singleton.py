#! python

import LocalException
import inspect


def caller_name(skip=2):
	"""Get a name of a caller in the format module.class.method
	`skip` specifies how many levels of stack to skip while getting caller
	name. skip=1 means "who calls me", skip=2 "who calls my caller" etc.
	An empty string is returned if skipped levels exceed stack height
	"""
	stack = inspect.stack()
	start = 0 + skip
	if len(stack) < start + 1:
		return ''
	parentframe = stack[start][0]
	name = []
	module = inspect.getmodule(parentframe)
	if module:
		name.append(module.__name__)
	if 'self' in parentframe.f_locals:
		name.append(parentframe.f_locals['self'].__class__.__name__)
	codename = parentframe.f_code.co_name
	del parentframe
	return ".".join(name)


class Singleton(type):
	_instances = {}
	def __call__(cls, *args, **kwargs):
		if cls not in cls._instances:
			cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
		return cls._instances[cls]
        
class UTConfig:
	__metaclass__ = Singleton
	def __init__(self):
		if not hasattr(self,'__cnf__'):
			self.__cnf__ = {}

	def setattr_ex(self,name,value,expand=None):
		if expand is None:			
			_name = caller_name()
			_name += '.'
			_name += name
			_value = value
		else:
			_name = expand +'.'+ name
			_value = value
		try:
			self.__cnf__[_name]=_value
		except:
			raise LocalException.LocalException('set value %s.%s = (%s) error'%(repr(name),repr(expand),repr(value)))
		return 1

	def getattr_ex(self,name,expand=None):		
		if expand is None:
			_name = caller_name()
			_name += '.'
			_name += name
		else:
			_name = expand +'.'+ name
		try:
			_v = self.__cnf__[_name]
			return _v
		except:
			raise LocalException.LocalException('get value %s.%s error'%(repr(name),repr(expand)))

	def delattr_ex(self,name,expand=None):
		if expand is None:
			_name = caller_name()
			_name += '.'
			_name += name
		else:
			_name = expand +'.'+ name
		try:
			del self.__cnf__[_name]
		except:
			raise LocalException.LocalException('del value %s.%s error'%(repr(name),repr(expand)))
		return


def main_test():
	ut = UTConfig()
	ut2 = UTConfig()
	assert(ut == ut2)
	ut.setattr_ex('ok','ok value')
	assert(ut.getattr_ex('ok') == 'ok value')
	ut.setattr_ex('fail','fail value','')
	assert(ut.getattr_ex('fail','') == 'fail value' )
	ut.setattr_ex('expand' ,'expand value','v')
	ok = 1
	try:
		v = ut.getattr_ex('expand')
	except LocalException.LocalException as e:
		ok = 0
	assert(ok == 0)

	ut.setattr_ex('expand','expand not value')
	assert( ut.getattr_ex('expand','__main__')=='expand not value')

if __name__ == '__main__':
	main_test()