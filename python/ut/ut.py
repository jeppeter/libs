import unittest
import sys
from optparse import OptionParser

class TestHelpSpot(unittest.TestCase):
    @classmethod
    def TestSpot_Opt_Parse_CallBack(cls,option, opt_str, value, parser):
        print 'option %s opt_str %s value %s parser %s'%(repr(option), repr(opt_str), repr(value), repr(parser))
        if opt_str == '-r' or opt_str == '--remote':
        	if value is None:
        		raise Exception('need more args for %s'%(opt_str))
        	cls._remote = value
        elif opt_str == '-P' or opt_str == '--play_timeout':
        	if value is None:
        		raise Exception('need more args for %s'%(opt_str))
        	cls._play_timeout = int(value)
        elif opt_str == '-S' or opt_str == '--setup_timeout':
        	if value is None:
        		raise Exception('need more args for %s'%(opt_str))
        	cls._setup_timeout = int(value)
        else:
        	raise Exception('can not parse %s'%(opt_str))
    @classmethod
    def AddArgs(cls,parser):
        parser.add_option('-P','--play_timeout',type="int",nargs=1,action='callback',callback=TestHelpSpot.TestSpot_Opt_Parse_CallBack,help='Set play timeout value ')
        parser.add_option('-S','--setup_timeout',type="int",nargs=1,action='callback',callback=TestHelpSpot.TestSpot_Opt_Parse_CallBack,help='Set setup timeout value ')
        parser.add_option('-r','--remote',type="string",nargs=1,action='callback',callback=TestHelpSpot.TestSpot_Opt_Parse_CallBack,help='Set remote value')
        cls._setup_timeout = 20
        cls._play_timeout = 60
        return
    @classmethod
    def ParseArgs(cls,argv):
        i = 0 
        print "argv %s"%(str(argv))
        while i < len(argv):
        	print '[%d] %s'%(i,argv[i])
        	if argv[i] == '-r' or argv[i] == '--remote':
        		if (i+1) >= len(argv):
        			raise Exception('need more args')
        		cls._remote = argv[i+1]
        		i += 1
        	i += 1
        pass
        
    @classmethod
    def setUpClass(cls):
        print "CTest Start class %s type(%s)"%(repr(cls),type(cls))
        cls._v = 6
        if hasattr(cls,'_remote'):
        	print "remote %s"%(cls._remote)
        pass
        
    @classmethod
    def tearDownClass(cls):
        print "CTest TearDown class"


    def test_version(self):
    	print "self _v %d remote %s "%(hasattr(self,'_v') and self._v or 0,hasattr(self,'_remote') and self._remote or "null")
        self.assertEqual(1,0)
    	pass

    def test_get_with_param(self):
    	pass

    def test_unknown_method(self):
        pass

class TestB(TestHelpSpot):
	@classmethod
	def setUpClass(cls):
		TestHelpSpot.setUpClass()
		print "Test B Class setup"

	@classmethod
	def tearDownClass(cls):
		print "Test B Class TearDown"
		TestHelpSpot.tearDownClass()

	def test_b(self):
		pass

def Parse_Callback(option, opt_str, value, parser):
	#print 'option %s opt_str %s value %s parser %s'%(repr(option), repr(opt_str), repr(value), repr(parser))
	if hasattr(parser.values,'unittest_args'):
		#print 'append unittest_args %s'%(opt_str)
		parser.values.unittest_args.append(opt_str)
		if value :
			parser.values.unittest_args.append(value)
	else:
		#print 'init unittest_args %s'%(opt_str)
		parser.values.unittest_args = []
		parser.values.unittest_args.append(sys.argv[0])
		parser.values.unittest_args.append(opt_str)
		if value :
			parser.values.unittest_args.append(value)
	

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
	args.add_option('-f','--failfast',action="callback",callback=Unittest_Args_Callback,nargs=0,help='fast fail')
	args.add_option('-c','--catch',action="callback",callback=Unittest_Args_Callback,nargs=0,help='catch interrupt from interactive')
	args.add_option('-b','--buffer',action="callback",callback=Unittest_Args_Callback,nargs=0,help='buffer stdout and stderr')
	return 


def TestCaseStart():
	args = OptionParser()
	Unittest_Args_Add(args)
	TestHelpSpot.AddArgs(args)	
	opt,nargs = args.parse_args(sys.argv[1:])	
	uargs = hasattr(opt,'unittest_args') and opt.unittest_args or [sys.argv[0]]
	for a in nargs :
		uargs.append(a)
	print 'opt %s nargs %s'%(repr(opt),repr(nargs))
	unittest.main( argv = uargs)
	
if __name__ == '__main__':
	TestCaseStart()
