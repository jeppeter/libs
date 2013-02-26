
import LocalException
import re
from optparse import OptionParser
import logging
import sys


class FormatGenBase(object):
	def GenerateResult(self,fmt,value):
		return value


class FormatGenInt(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**32
			maxhv = 2**31
			iv = int(value)
			rpat = re.compile('%%x')
			isx = 0
			if rpat.search(fmt):
				isx = 1
			if iv >= maxv and isx == 0:
				iv %= maxv

			if iv >= maxhv and isx == 0:
				iv = maxhv - 1
			if isx == 1:
				sv = '%x'%(iv)
			else:
				sv = '%d'%(iv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,repr(value)))

class FormatGenString(FormatGenBase):
	pass

class FormatGenFloat(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			fv = float(value)
			m = re.match('%\.([\d]+)f',fmt)
			if m and m.group(1):
				n  = int(m.group(1))
				if n <= 0 or n>=5:
					raise LocalException.LocalException('can not format(%d)(%s)'%(n,fmt))
				fmtv = '%%.%df'%(n)
			else:
				raise LocalException.LocalException('not valid fmt (%s)'%(fmt))
			sv = fmtv%(fv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenDouble(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			fv = float(value)
			m = re.match('%\.([\d]+)g',fmt)
			if m and m.group(1):
				n = int(m.group(1))
				if n <= 0 or n >= 5:
					raise LocalException.LocalException('fmt (%s) size >=5 or <=0'%(fmt))
				fmtv = '%%.%dg'%(n)
				sv = fmtv%(fv)
				return sv
			else:
				raise LocalException.LocalException('can not right fmt (%s) for Double'%(fmt))
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenLongLong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**64
			maxhv = 2**63
			curv = int(value)
			if curv >= maxv :
				curv %= maxv
			m = re.match('%%llx',fmt)
			if  m:
				# it is %llx format
				#logging.info('curv (%d) '%(curv))
				sv = '%x'%(curv)
			else:
				# it is %lld format now to give the long value
				if curv >= maxhv:
					curv = curv - maxv				
				sv = '%d'%(curv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenULongLong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**64
			curv = int(value)
			if curv >= maxv:
				curv %= maxv			
			m = re.match('%%llx',fmt)
			if m:
				# it is %llx format
				sv = '%x'%(curv)
			else:
				sv = '%d'%(curv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenLong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**32
			maxhv = 2**31
			curv = long(value)
			if curv >= maxv:
				curv %= maxv
			rpat = re.compile('%%lx')
			if rpat.search(fmt):
				sv = '%x'%(curv)
			else:
				if curv >= maxhv:
					curv = curv - maxv
				sv = '%d'%(curv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenULong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**32
			curv = long(value)
			if curv >= maxv:
				curv %= maxv
			rpat = re.compile('%%lx')
			if rpat.search(fmt):
				sv = '%x'%(curv)
			else:
				sv = '%d'%(curv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))
		

import random
import time

CharacterUse='abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789\'\"='
#CharacterUse='ab\'\"='
NumberUse='0123456789'

class RandomFormatGen:
	def __init__(self,seed=None):
		if seed is None:
			seed = time.time()
		random.seed(seed)
		self.__fmt = None
		self.__opts = None
		self.__sv = None
		self.__argv = None
		self._rseed = seed
		return

	def GenValue(self):
		global CharacterUse
		random.seed(self._rseed)
		k = random.randint(0,5)		
		if k == 0:
			f = FormatGenInt()
			v = random.randint(0,2**33)
			fmt = random.randint(0,1) and '%d' or '%x'
			sfmt = '-i'
			s = '%d'%(v)
		elif k == 1:
			f = FormatGenString()
			l = len(CharacterUse) -1
			fmt = '%s'
			s = ''
			sfmt = '-s'			
			for i in xrange(35):
				j = random.randint(0,l)
				s += CharacterUse[j]
		elif k == 2:
			f = FormatGenDouble()
			dot = 0
			s = ''
			sfmt = '-d'
			for i in xrange(30):
				j = random.randint(0,10)
				if j == 10 and dot == 1:
					j = 0
				elif j == 10:
					if len(s) == 0:
						s += '0'
					s += '.'
					j = 0
					dot = 1
					
				s += NumberUse[j]
			i = random.randint(1,4)
			fmt = '%%.%dg'%(i)			
		elif k == 3:
			f = FormatGenFloat()
			dot = 0			
			s = ''
			sfmt = '-F'			
			for i in xrange(30):
				j = random.randint(0,10)
				if j == 10 and dot == 1:
					j = 0
				elif j == 10:
					if len(s) == 0:
						s += '0'
					s += '.'
					j = 0
					dot = 1
				s += NumberUse[j]
			i = random.randint(1,4)
			fmt = '%%.%df'%(i)			
		elif k == 4:
			f = FormatGenLongLong()
			s = ''
			sfmt = '-ll'
			for i in xrange(50):
				j = random.randint(0,9)
				s += NumberUse[j]
			fmt = random.randint(0,1) and '%llx' or '%lld'
		elif k == 5:
			f = FormatGenULongLong()
			s = ''
			sfmt = '-ull'
			for i in xrange(50):
				j = random.randint(0,9)
				s += NumberUse[j]
			fmt = random.randint(0,1) and '%llx' or '%lld'
		else:
			logging.error('k (%d)'%(k))
			assert(0!=0)
		sv = f.GenerateResult(fmt,s)
		self.__fmt = fmt
		self.__opts = sfmt
		self.__argv = s
		self.__sv = sv
		self._rseed += 1
		return

	def GetFmt(self):
		return self.__fmt

	def GetOpt(self):
		return self.__opts

	def GetArgv(self):
		return self.__argv

	def GetSv(self):
		return self.__sv

	def __del__(self):
		pass


import unittest

class FormatGenUnittest(unittest.TestCase):
	def test_String1(self):
		f = FormatGenString()
		svi = 'ssvvddd'
		fmt = '%%s'
		v = f.GenerateResult(fmt,svi)
		self.assertEqual(svi,v)

	def test_String2(self):
		f = FormatGenString()
		fmt = '%%s'
		svi = 'cccfffvv%s'%('eevvee')
		v = f.GenerateResult(fmt,svi)
		self.assertEqual(svi,v)

	def test_LongLong1(self):
		f = FormatGenLongLong()
		ll = 2**64+1
		maxv = 2**64
		llv = ll
		fmt = '%%lld'
		if ll > maxv :
			llv = ll % maxv
		svi = '%d'%(llv)
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(svi,v)

		return 
	def test_LongLong2(self):
		f = FormatGenLongLong()
		ll = 2**63 + 1
		maxhv = 2**63
		maxv = 2**64
		llv = ll
		fmt = '%%lld'
		if ll >= maxhv and ll < maxv:
			llv = ll - maxv
		elif ll >= maxv:
			llv = ll % maxv
			if llv >= maxhv :
				llv = llv - maxv
		svi = '%d'%(llv)
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(v,svi)
		return
	def test_LongLong3(self):
		f = FormatGenLongLong()
		ll = 2**63+1
		maxv = 2**64
		llv = ll
		fmt = '%%llx'
		if ll >= maxv :
			llv = ll % maxv
		svi = '%x'%(llv)
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(svi,v)
		return
		
	def test_Int1(self):
		f = FormatGenInt()
		i = 2**32 - 1
		maxhv = 2**31 
		maxv = 2**32
		fmt = '%%d'
		s = '%d'%(i)
		li = i
		if i >= maxhv:
			li = i % maxv
			li = li - maxv

		svi = '%d'%(li)		
		v = f.GenerateResult(fmt,s)
		self.assertEqual(svi,v)

		return 
	def test_Int2(self):
		f = FormatGenInt()
		i = 2**32 + 1
		maxhv = 2**31 
		maxv = 2**32
		fmt = '%%d'
		li = i
		if i >= maxv:
			li = i % maxv
		svi = '%d'%(li)	
		v = f.GenerateResult(fmt,i)
		self.assertEqual(svi,v)

		return
	def test_Double1(self):
		f = FormatGenDouble()
		d = 33.221225554
		s = '%f'%(d)
		fmt = '%%.3g'
		fmtv = '%.3f'
		svi = fmtv%(d)
		v = f.GenerateResult(fmt,s)
		self.assertEqual(svi,v)
		return 
	def test_Double2(self):
		f = FormatGenDouble()
		d = 33.22333555533
		s = '%f'%(d)
		fmt = '%.10g'
		ok = 0
		try:
			f.GenerateResult(fmt,s)
		except LocalException.LocalException as e:
			ok = 1
		self.assertEqual(ok,1)
		return
	def test_Double3(self):
		f = FormatGenDouble()
		d = 33.22333555533
		s = '%f'%(d)
		fmt = '%g'
		ok = 0
		try:
			f.GenerateResult(fmt,s)
		except LocalException.LocalException as e:
			ok = 1
		self.assertEqual(ok,1)
		return 
	def test_Double4(self):
		f = FormatGenDouble()
		s = '33.22333555533'
		fmt = '%%.3g'
		v = '33.223'
		svi = f.GenerateResult(fmt,s)
		self.assertEqual(v ,svi)
		return 

	def test_Float1(self):
		f = FormatGenFloat()
		d = 33.221225554
		s = '%f'%(d)
		fmt = '%.3f'
		fmtv = '%.3f'
		svi = fmtv%(d)
		v = f.GenerateResult(fmt,s)
		self.assertEqual(svi,v)
		return 
	def test_Float2(self):
		f = FormatGenFloat()
		d = 33.22333555533
		s = '%f'%(d)
		fmt = '%.10f'
		ok = 0
		try:
			f.GenerateResult(fmt,s)
		except LocalException.LocalException as e:
			ok = 1
		self.assertEqual(ok,1)
		return
	def test_Float3(self):
		f = FormatGenFloat()
		d = 33.22333555533
		s = '%f'%(d)
		fmt = '%f'
		ok = 0
		try:
			f.GenerateResult(fmt,s)
		except LocalException.LocalException as e:
			ok = 1
		self.assertEqual(ok,1)
		return
	def test_ULongLong1(self):
		f = FormatGenULongLong()
		ll = 2**63+1
		svll = '%d'%(ll)
		fmt = '%%lld'
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(v,svll)
		return
	def test_ULongLong2(self):
		f = FormatGenULongLong()
		maxv = 2**64
		ll = 2**64+1
		llv = ll
		if ll >= maxv :
			llv = ll - maxv
		svll = '%d'%(llv)
		fmt = '%%lld'
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(v,svll)
		return

	def test_ULongLong3(self):
		f = FormatGenULongLong()
		ll = 2**63+1
		svll = '%x'%(ll)
		fmt = '%%llx'
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(v,svll)
		return
		
	def test_ULongLong4(self):
		f = FormatGenULongLong()
		maxv = 2**64
		ll = 2**64+1
		llv = ll
		if ll >= maxv :
			llv = ll - maxv
		svll = '%x'%(llv)
		fmt = '%%llx'
		v = f.GenerateResult(fmt,ll)
		self.assertEqual(v,svll)
		return

	def test_Long1(self):
		f = FormatGenLong()
		maxv = 2**32
		l = 2**32 + 13
		lv = l
		if l >= maxv :
			lv = l %maxv
		slv = '%x'%(lv)
		fmt = '%%lx'
		sv = f.GenerateResult(fmt,l)
		self.assertEqual(slv,sv)
		return

	def test_Long2(self):
		f = FormatGenLong()
		maxv = 2**32
		maxhv = 2**31
		l = 2**32 - 13
		lv = l
		if l >= maxv :
			lv = l %maxv

		if lv >= maxhv:
			lv = lv - maxv
		slv = '%d'%(lv)
		fmt = '%%ld'
		sv = f.GenerateResult(fmt,l)
		self.assertEqual(slv,sv)
		return

	def test_Long3(self):
		f = FormatGenLong()
		maxv = 2**32
		maxhv = 2**31
		l = 2**32 - 13
		lv = l
		if l >= maxv :
			lv = l %maxv

		slv = '%x'%(lv)
		fmt = '%%lx'
		sv = f.GenerateResult(fmt,l)
		self.assertEqual(slv,sv)
		return


	def test_ULong1(self):
		f = FormatGenULong()
		maxv = 2**32
		maxhv = 2**31
		l = 2**32 - 13
		lv = l
		if l >= maxv :
			lv = l %maxv

		slv = '%d'%(lv)
		fmt = '%%ld'
		sv = f.GenerateResult(fmt,l)
		self.assertEqual(slv,sv)
		return
		



def Unittest_Args_Callback(option, opt_str, value, parser):
	if hasattr(parser.values,'unittest_args'):
		parser.values.unittest_args.append(opt_str)
		if value :
			parser.values.unittest_args.append(value)
	else:
		parser.values.unittest_args = []
		parser.values.unittest_args.append(sys.argv[0])
		parser.values.unittest_args.append(opt_str)
		if value :
			parser.values.unittest_args.append(value)


def Unittest_Args_Add(args):
	args.add_option('-v','--verbose',action="callback",callback=Unittest_Args_Callback,nargs=0,help='verbose mode')
	args.add_option('-q','--quiet',action="callback",callback=Unittest_Args_Callback,nargs=0,help='quiet mode')
	args.add_option('-f','--failfast',action="callback",callback=Unittest_Args_Callback,nargs=0,help='fail fast')
	args.add_option('-c','--catch',action="callback",callback=Unittest_Args_Callback,nargs=0,help='catch interrupt from interactive')
	args.add_option('-b','--buffer',action="callback",callback=Unittest_Args_Callback,nargs=0,help='buffer for stdout and stderr')
	return 

def MainTest():
	args = OptionParser()
	Unittest_Args_Add(args)
	# ok we should get the opt
	opt,restargs = args.parse_args(sys.argv[1:])
	uargs = hasattr(opt,'unittest_args') and opt.unittest_args or [sys.argv[0]]
	for a in restargs:
		uargs.append(a)
	if '-v' in uargs or '--verbose' in uargs:
		logging.basicConfig(level=logging.INFO, format='%(levelname)s %(filename)s:%(lineno)d - - %(asctime)s %(message)s', datefmt='[%b %d %H:%M:%S]')
	unittest.main( argv = uargs)


if __name__ == '__main__':
	MainTest()	
