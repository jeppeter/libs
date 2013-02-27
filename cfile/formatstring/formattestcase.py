#! python
import re
import sys
test_cases=(
	('-f','hello'),
	('-f','hello %s','-s','world'),
	('-f','hello %d','-i',10),
	('-f','hello %d %.1f','-i',10,'-F',3.33333),
	('-f','hello %g','-d',30.22),
	('-f','new good %.3f %s %ld %lu','-F',3002.11114,'-s','new file','-l',33385,'-ul',33365),
)

def DebugString(s):
	sys.stderr.write(s+'\n')

def GenerateFormatString(args):
	rets = ''
	try:
		rets = args[0]
		rets = rets%(args[1:])
	except:
		try :
			rets = args[0]
		except:
			rets = ''
	return rets

def HasToQuote(s):
	rs = re.compile('[\s]+')
	rq = re.compile('\"')
	r = 0
	if rs.search(s)or rq.search(s) :
		r = 1
	return r


def QuoteString(v):
	if isinstance(v,str):
		s = v
	else:
		s = str(v)
	s = s.replace('\"','\\\"')
	if HasToQuote(s):
		s = '\"' + s + '\"'
	return s
	

def GenerateTestCase(tmpdir,args):
	farg = []
	if len(args) < 1:
		raise Exception('args less than 1')
	if args[0] != '-f':
		raise Exception('args[0] (%s) not -f'%(str(args[0])))
	if len(args) % 2:
		raise Exception('args len %d not even size'%(len(args)))
	for i in xrange(1,len(args),2):		
		farg.append(args[i])
	s = GenerateFormatString(tuple(farg))
	newargs = list(args)
	newargs.append('-r')
	newargs.append(s)
	rets = tmpdir+'testformat '
	for i in xrange(0,len(newargs)):
		if i > 0:
			rets += ' '
		rets += QuoteString(newargs[i])	
	return rets



def GenerateTests(tmpdir,args):
	for i in xrange(0,len(args)):		
		s = GenerateTestCase(tmpdir,args[i])
		print s
	

GenerateTests("./",test_cases)
