#! python

import inspect

class LocalException(Exception):
	def __init__(self,msg):
		_f = inspect.stack()[1]
		_msg = '%s:%s %s'%(_f[1],_f[2],msg)
		super(Exception,self).__init__(_msg)

