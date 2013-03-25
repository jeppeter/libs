#! python

import sys

class UTConfig:
	def __init__(self,**kwargs):
		if k in kwargs.keys():
			setattr(self,k,kwargs[k])
		if hasattr(self,'path')
			sys.path.extend(getattr(self,'path'))

	def __del__(self):
		pass

	
