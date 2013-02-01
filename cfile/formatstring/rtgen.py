#! python
###########################
#
#
#  this file is to generate the random test
#  this is the case of random
#
#
#
#
#
#
###########################
import random
import time
import ctypes

class RandomValue:
	def __init__(self):
		self._functable = {
			'-i' : self.IntValue
		}

	def IntValue(self,maxvalue):
		random.seed(time.time())		
		return random.randint(0,maxvalue)

	def LongValue(self,maxvalue):
		random.seed(time.time())		
		return random.randint(0,maxvalue)

	def UIntValue(self,maxvalue):
		random.seed(time.time())		
		return random.randint(0,maxvalue)
		
CharacterUse='abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789\"\'='
GenrateFuncs={
	'-i' : GenerateInteger,
	
}

def GenRandomChar(numchars,charsetuse):
	random.seed(time.time())
	s = ''
	for i in xrange(0,numchars):		
		s += charsetuse[random.randint(0,len(charsetuse)-1)]
	return s

def GenRndCase(maxargs):
	# now to give the case use
	random.seed(time.time())
	fmt = ''
	numargs = random.randint(0,maxargs-1)
	for i in xrange(0,numargs):
		

print GenRandomChar(30,CharacterUse)
