import sys

def GetBinary(num,fl):
	s = bin(num)
	s = s.replace('0b','')
	# the format is 0bxxx so it will replace
	if len(s) < fl:
		ll = fl - len(s)
		a0 = '0' * ll
		av = a0 + s
		s = av
	return s

def GetLL(prefix,s):
	ll = len(s)
	i = 0
	rets = '( 0'
	while i < ll:
		if s[i] == '1':
			rets += (' | %s_%d'%(prefix,(ll - i)) )
		i += 1
	rets += ')'
	return rets


def BinarySearch(prefix,nshift):
	maxnum = (1 << nshift)
	i = 0
	ts = '/*for %d shift*/\n'%(nshift)
	while i < maxnum :
		ts += '#define M%s_'%(prefix)
		bn = GetBinary(i,nshift)
		ts += bn
		ts += '        '
		dv = GetLL(prefix,bn)
		ts += dv
		ts += '\n'
		i += 1
	return ts

def BinUp(prefix,nshift):
	i=0
	ts = ''
	while i < nshift:
		ts += BinarySearch(prefix,i)
		ts += '\n\n'
		i += 1
	return ts

def LLGen(prefix,n):
	if n < 1 :
		return ''
	str = '#define %s_%d       (1 << %d)\n'%(prefix,n,(n-1))
	return str
def MaxFormatGen(n):
	if n < 2:
		raise Exception('Could not accept less than 1')
	str = '#define  MAX_FORMAT_ARGS   %d\n'%(n)
	return str

if __name__ =='__main__':
	maxnum = int(sys.argv[1])
	s = MaxFormatGen(maxnum)
	print s
	for i in xrange(maxnum+1):
		print LLGen('LL',i)
	for i in xrange(maxnum+1):
		ts = BinUp('LL',i)		
		print ts
	for i in xrange(maxnum+1):
		print LLGen('FF',i)
	for i in xrange(maxnum+1):
		ts = BinUp('FF',i)
		print ts
