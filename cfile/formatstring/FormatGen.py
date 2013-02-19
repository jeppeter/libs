
import LocalException
import re
from ctypes import c_longlong as longlong
class FormatGenBase(object):
	def GenerateResult(self,fmt,value):
		return value


class FormatGenInt(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2*32 - 1
			
			iv = int(value):
			if iv > maxv:
				iv %= maxv
			sv = fmt%(iv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenString(FormatGenBase):
	pass

class FormatGenFloat(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			fv = float(value)
			m = re.match(r'\%\.([\d]+)f',fmt)
			if m and m.group(1):
				pass
			else:
				raise LocalException.LocalException('not valid fmt (%s)'%(fmt))
			sv = fmt%(fv)
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenDouble(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:			
			fv = float(value)
			m = re.match(r'\%\.([\d]+)g',fmt)
			if m and m.group(1):
				n = int(m.group(1))
				if n <= 0 or n >= 5:
					raise LocalException.LocalException('fmt (%s) size >=5 or <=0'%(fmt))
				fmtv = '\%\.%df'%n
				sv = fmtv%(fv)
				return sv
			else:
				raise LocalException.LocalException('can not right fmt (%s) for Double'%(fmt))
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenLongLong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**63-1
			curv = int(value)
			if curv > maxv :
				curv %= maxv
			m = re.match('\%llx',fmt)
			if  m:
				# it is %llx format
				sv = '%x'%(curv)
			else:
				# it is %lld format now to give the long value
				llv = longlong(curv)
				sv = '%d'%(llv.value)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))

class FormatGenULongLong(FormatGenBase):
	def GenerateResult(self,fmt,value):
		try:
			maxv = 2**63-1
			curv = int(value)
			if curv > maxv:
				curv %= maxv
			m = re.match('%llx',fmt)
			if m:
				# it is %llx format
				sv = '%x'%(curv)
			else:
				sv = '%d'%(curv)
			return sv
		except:
			raise LocalException.LocalException('can not change %s %s value'%(fmt,value))