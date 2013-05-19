#! python
import random
import time

CharacterUse='abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789'
#CharacterUse='ab\'\"='
NumberUse='0123456789'

def InitRandom():
	random.seed(time.time())
	return

def GenRandomChars(num):
	s = ''
	slen = len(CharacterUse)
	slen -= 1
	for i in xrange(num):
		s += CharacterUse[random.randint(0,slen)]
	return s

def GenRandomNum(num,minnum=0):
	return random.randint(minnum,num-1)