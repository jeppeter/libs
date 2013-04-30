#! python

import sys
import inspect

class LocalException(Exception):
	def __init__(self,msg):
		t,v,tb = sys.exc_info()
		if isinstance(v,LocalException):
			super(Exception,self).__init__(str(v))
		else:
			_f = inspect.stack()[1]		
			_msg = '%s:%s (%s)(%s)'%(_f[1],_f[2],sys.exc_info(),msg)
			super(Exception,self).__init__(_msg)

def testLocalException(first,second):	
	try:
		raise LocalException(first)
	except:
		raise LocalException(second)


if __name__ == '__main__':
	import re
	ok = 0
	first = 'First'
	second = 'Second'
	try:
		testLocalException(first,second)
	except LocalException as e:
		es = str(e)
		fpat = re.compile(first)
		spat = re.compile(second)
		if fpat.search(es) and  spat.search(es) is None:
			ok = 1
	assert(ok == 1)
	print 'test LocalException Ok'
