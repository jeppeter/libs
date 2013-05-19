#! python

import logging
import xunit.utils.cls
import xunit.config
import sys
import StringIO
import inspect
import atexit
import time
import datetime
import xunit.config

from xunit.utils import exception
from xunit.utils import cls
from xunit.case import XUnitCase
from xunit.config import XUnitConfig

class NotDefinedClassMethodException(exception.XUnitException):
	pass

CRITICAL_LEVEL=1
ERROR_LEVEL=2
WARNING_LEVEL=3
INFO_LEVEL=4
DEBUG_LEVEL=5

def DebugString(msg,level=1):
	_f = inspect.stack()[level]
	_msg = '[%s:%s] %s'%(_f[1],_f[2],msg)
	sys.stderr.write(_msg+'\n')


MAX_CASE_LEN = 70
MAX_CASE_NAME_LEN = 55


class AbstractLogger:
	def __init__(self,cn):
		pass

	def __del__(self):
		pass

	def SetLevel(self,level=WARNING_LEVEL):
		raise NotDefinedClassMethodException('not defined SetLevel')
	def SetOutput(self,output=1):
		raise NotDefinedClassMethodException('not defined SetOutput')	
	def Info(self,msg):
		raise NotDefinedClassMethodException('not defined Info')
	def Warn(self,msg):
		raise NotDefinedClassMethodException('not defined Warn')
	def Error(self,msg):
		raise NotDefinedClassMethodException('not defined Error')
	def Debug(self,msg):
		raise NotDefinedClassMethodException('not defined Debug')
	def Flush(self):
		raise NotDefinedClassMethodException('not defined Flush')
	def TestStart(self,msg):
		raise NotDefinedClassMethodException('not defined TestStart')
	def CaseStart(self,msg):
		raise NotDefinedClassMethodException('not defined CaseStart')
	def CaseFail(self,msg):
		raise NotDefinedClassMethodException('not defined CaseFail')
	def CaseError(self,msg):
		raise NotDefinedClassMethodException('not defined CaseError')
	def CaseSucc(self,msg):
		raise NotDefinedClassMethodException('not defined CaseSucc')
	def CaseSkip(self,msg):
		raise NotDefinedClassMethodException('not defined CaseSkip')
	def CaseEnd(self,msg):
		raise NotDefinedClassMethodException('not defined CaseEnd')
	def TestEnd(self,msg):
		raise NotDefinedClassMethodException('not defined TestEnd')
	def write(self,msg):
		raise NotDefinedClassMethodException('not defined write')
	def flush(self):
		raise NotDefinedClassMethodException('not defined flush')

class BaseLogger(AbstractLogger):
	def __init__(self,cn):
		'''
		    init the logger ,and we do this by the string 
		    input
		    @cn class name of logger get
		'''
		self.__strio = None
		self.__level = WARNING_LEVEL
		self.__output = 1
		self.__outfh = sys.stdout
		self.__ResetStrLogger()
		

	def __ResetStrLogger(self):
		if self.__strio :
			self.__strio.close()
			del self.__strio
			self.__strio = None

		# now to set the logger format
		try:
			# this will call error on delete function call sequence
			self.__strio = StringIO.StringIO()
		except:
			pass
		self.__caselen = 0
		return

	def __flush(self):
		v = ''
		if self.__strio:
			v = self.__strio.getvalue()
			if len(v) > 0 and self.__output > 0:
				self.__outfh.write(v)
				self.__outfh.flush()
			if len(v) == 0:
				v = ''
		return v

	def __del__(self):
		self.__flush()
		del self.__strio
		self.__strio = None
		self.__outfh = None
		return

	def SetLevel(self,level=WARNING_LEVEL):
		oldlevel = self.__level
		self.__level = level
		return oldlevel
	def SetOutput(self,output=1):
		oldoutput = self.__output
		self.__output = output
		return oldoutput
	
	def Info(self,msg):
		if self.__level >= INFO_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s\n'%(_f[1],_f[2],msg)
			self.__strio.write(_msg)

	def Warn(self,msg):
		if self.__level >= WARNING_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s\n'%(_f[1],_f[2],msg)
			self.__strio.write(_msg)

	def Error(self,msg):
		if self.__level >= ERROR_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s\n'%(_f[1],_f[2],msg)
			self.__strio.write(_msg)
		

	def Debug(self,msg):
		if self.__level >= DEBUG_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s\n'%(_f[1],_f[2],msg)
			self.__strio.write(_msg)

	def Flush(self):
		value = ''
		if self.__strio :
			value = self.__flush()
			self.__ResetStrLogger()
		return value

	def TestStart(self,msg):
		self.__caselen = 0
		return

	def CaseStart(self,msg):
		_msg = msg
		self.__caselen = 0
		if len(_msg) > MAX_CASE_NAME_LEN:
			_tmp = '['
			_tmp += _msg[:MAX_CASE_NAME_LEN]
			_msg = _tmp
		else:
			cnt = MAX_CASE_NAME_LEN - len(_msg)
			_tmp = '['
			_tmp += ' ' * cnt
			_tmp += _msg
			_msg =_tmp 
		self.__outfh.write('%s'%(_msg))
		self.__outfh.flush()
		self.__caselen = len(_msg)
		return

	def CaseFail(self,msg):
		_msg = msg
		if len(_msg) > (MAX_CASE_LEN - MAX_CASE_NAME_LEN):
			_tmp = '\t'
			_tmp += _msg[:(MAX_CASE_LEN - MAX_CASE_NAME_LEN)]
			_msg = _tmp
		else:
			cnt =MAX_CASE_LEN - MAX_CASE_NAME_LEN - len(_msg)
			_tmp = '\t'
			_tmp += ' '*cnt
			_tmp += _msg
			_msg = _tmp
			
		self.__outfh.write('%s'%(_msg))
		self.__outfh.flush()
		self.__caselen += len(_msg)
		return

	def CaseError(self,msg):
		_msg = msg
		if len(_msg) > (MAX_CASE_LEN - MAX_CASE_NAME_LEN):
			_tmp = '\t'
			_tmp += _msg[:(MAX_CASE_LEN - MAX_CASE_NAME_LEN)]
			_msg = _tmp
		else:
			cnt =MAX_CASE_LEN - MAX_CASE_NAME_LEN - len(_msg)
			_tmp = '\t'
			_tmp += ' '*cnt
			_tmp += _msg
			_msg = _tmp
		self.__outfh.write('%s'%(_msg))
		self.__outfh.flush()
		self.__caselen += len(_msg)
		return

	def CaseSucc(self,msg):
		_msg = msg
		if len(_msg) > (MAX_CASE_LEN - MAX_CASE_NAME_LEN):
			_tmp = '\t'
			_tmp += _msg[:(MAX_CASE_LEN - MAX_CASE_NAME_LEN)]
			_msg = _tmp
		else:
			cnt =MAX_CASE_LEN - MAX_CASE_NAME_LEN - len(_msg)
			_tmp = '\t'
			_tmp += ' '*cnt
			_tmp += _msg
			_msg = _tmp
		self.__outfh.write('%s'%(_msg))
		self.__outfh.flush()
		self.__caselen += len(_msg)
		return
	def CaseSkip(self,msg):
		_msg = msg
		if len(_msg) > (MAX_CASE_LEN - MAX_CASE_NAME_LEN):
			_tmp = '\t'
			_tmp += _msg[:(MAX_CASE_LEN - MAX_CASE_NAME_LEN)]
			_msg = _tmp
		else:
			cnt =MAX_CASE_LEN - MAX_CASE_NAME_LEN - len(_msg)
			_tmp = '\t'
			_tmp += ' '*cnt
			_tmp += _msg
			_msg = _tmp
		self.__outfh.write('%s'%(_msg))
		self.__outfh.flush()
		self.__caselen += len(_msg)
		return
	def CaseEnd(self,msg):
		self.__outfh.write(']\n')
		self.__outfh.flush()
		self.__caselen += 2
		return

	def TestEnd(self,msg):
		self.__outfh.write('\n%s\n'%(msg))
		self.__caselen = 0
		return

	def write(self,msg):
		if self.__level >= DEBUG_LEVEL:
			self.__strio.write(msg)
		return
	def flush(self):
		return self.Flush()
		


class XmlLogger(AbstractLogger):
	def __init__(self,cn,stream=sys.stdout):
		'''
		    init the logger ,and we do this by the string 
		    input
		    @cn class name of logger get
		'''
		self.__strio = None
		self.__level = WARNING_LEVEL
		self.__output = 1
		self.__outfh = stream
		self.__cn = cn
		self.__ResetStrLogger()
		

	def __ResetStrLogger(self):
		if self.__strio :
			self.__strio.close()
			del self.__strio
			self.__strio = None

		# now to set the logger format
		try:
			# this will call error on delete function call sequence
			self.__strio = StringIO.StringIO()
		except:
			pass
		self.__caselen = 0
		return

	def __flush(self):
		v = ''
		if self.__strio:
			v = self.__strio.getvalue()			
			if len(v) > 0 and self.__output > 0 and self.__outfh:
				self.__outfh.write(v)
				self.__outfh.flush()
			if len(v) == 0:
				v = ''
		return v

	def __del__(self):
		self.__flush()
		del self.__strio
		self.__strio = None
		self.__outfh = None
		return

	def __GetTestCaseName(self):
		# default value is __main__ ,if we find ,we do not use this
		stks = inspect.stack()
		for i in xrange(0,len(stks)-1):
			frm = stks[i]
			mm = inspect.getmodule(frm[0])
			fc = frm[0].f_code
			fn = frm[3]
			if fn.lower().startswith('test_'):
				# if we find the test_ case name ,so we should test whether it is the XUnitCase class functions,
				# if so we return the whole module.class:function format
				_cls = fc.co_varnames[0]
				inst = frm[0].f_locals[_cls]
				# it is the class we search for ,so we return it
				if issubclass(inst.__class__, XUnitCase):
					cn = cls.GetClassName(inst.__class__) + ':' + fn
					return cn
		# we do not get the name, so we should get the class
		for i in xrange(0,len(stks)-1):
			frm = stks[i]
			mm = inspect.getmodule(frm[0])
			fc = frm[0].f_code
			fn = frm[3]
			_cls = fc.co_varnames[0]
			inst = frm[0].f_locals[_cls]
			# it is the class we search for ,so we return it
			if issubclass(inst.__class__, XUnitCase):
				cn = cls.GetClassName(inst.__class__)
				return cn
		# nothing find ,so we should return __main__
		return '__main__'

	def SetLevel(self,level=WARNING_LEVEL):
		oldlevel = self.__level
		self.__level = level
		return oldlevel
	def SetOutput(self,output=1):
		oldoutput = self.__output
		self.__output = output
		return oldoutput
	def Info(self,msg):
		if self.__level >= INFO_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s'%(_f[1],_f[2],msg)
			_msg = '<infomsg><func>%s</func><msg>%s</msg></infomsg>'%(self.__GetTestCaseName(),_msg)
			self.__strio.write(_msg+'\n')
	def Warn(self,msg):
		if self.__level >= WARNING_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s'%(_f[1],_f[2],msg)
			_msg = '<warnmsg><func>%s</func><msg>%s</msg></warnmsg>'%(self.__GetTestCaseName(),_msg)
			self.__strio.write(_msg+'\n')
	def Error(self,msg):
		if self.__level >= ERROR_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s'%(_f[1],_f[2],msg)
			_msg = '<errormsg><func>%s</func><msg>%s</msg></errormsg>'%(self.__GetTestCaseName(),_msg)
			self.__strio.write(_msg+'\n')
	def Debug(self,msg):
		if self.__level >= DEBUG_LEVEL:
			_f = inspect.stack()[1]
			_msg = '[%s:%s] %s'%(_f[1],_f[2],msg)
			self.__GetTestCaseName()
			_msg = '<debugmsg><func>%s</func><msg>%s</msg></debugmsg>'%(self.__GetTestCaseName(),_msg)
			self.__strio.write(_msg+'\n')
	def Flush(self):
		v = self.__flush()
		self.__ResetStrLogger()
		return v
	def TestStart(self,msg):
		utcfg = XUnitConfig()
		_msg = '<?xml version="1.0"?>\n'
		_msg += '<xunittest>\n<starttime>%s</starttime>\n<configfile>%s</configfile>\n<testsubject>%s</testsubject>\n'%(datetime.datetime.now(),utcfg.GetConfigFile(),msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseStart(self,msg):
		_msg = '<xunitcase ><starttime>%s</starttime><func>%s</func>\n'%(datetime.datetime.now(),msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseFail(self,msg):
		_msg = '<result><indication>fail</indication><msg>%s</msg></result>\n'%(msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseError(self,msg):
		_msg = '<result><indication>error</indication><msg>%s</msg></result>\n'%(msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseSucc(self,msg):
		_msg = '<result><indication>succ</indication><msg>%s</msg></result>\n'%(msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseSkip(self,msg):
		_msg = '<result><indication>skip</indication><msg>%s</msg></result>\n'%(msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def CaseEnd(self,msg):
		_msg = '<endtime>%s</endtime><msg>%s</msg></xunitcase>\n'%(datetime.datetime.now(),msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
	def TestEnd(self,msg):
		_msg = '<endtime>%s</endtime>\n<msg>%s</msg></xunittest>\n'%(datetime.datetime.now(),msg)
		if self.__outfh and self.__output > 0:
			self.__outfh.write(_msg)
			self.__outfh.flush()
		return
		
	def write(self,msg):
		self.Debug(msg)
		self.Flush()
		return
	def flush(self):
		self.Flush()
		return
	



class _AdvLogger:
	default_xmllog = None
	default_xmlhandler = None
	default_xmllevel = 3
	default_baselevel = 3
	def __GetXmlLogDefault(self):
		utcfg = xunit.config.XUnitConfig()		
		_AdvLogger.default_xmllog = utcfg.GetValue('global','xmllog','')
		_AdvLogger.default_xmllevel = int(utcfg.GetValue('global','xmllevel','3'))
		#logging.info('_AdvLogger.default_xmllevel %d'%(_AdvLogger.default_xmllevel))
		return

	def __init__(self,cn):
		self.__loggers = []
		self.__xmlhandler = None
		if _AdvLogger.default_xmllog is None:
			self.__GetXmlLogDefault()

		# now to open the log file
		if len(_AdvLogger.default_xmllog) >0 and  _AdvLogger.default_xmlhandler is None:
			_AdvLogger.default_xmlhandler = open(_AdvLogger.default_xmllog,"w")

		_fh = _AdvLogger.default_xmlhandler
		_lv = _AdvLogger.default_xmllevel
		sec = '.' + cn
		utcfg = xunit.config.XUnitConfig()
		v = utcfg.GetValue(sec,'xmllog','')
		# default level is warning
		_lv = utcfg.GetValue(sec,'xmllevel',3)
		if len(v) > 0:
			self.__xmlhandler = open(v,'w')
			_fh = self.__xmlhandler
		_logger = BaseLogger(cn)
		self.__loggers.append(_logger)
		if _fh :
			_logger = XmlLogger(cn,_fh)			
			self.__loggers.append(_logger)
		self.SetLevel(_lv)
		return

	def __del__(self):
		while len(self.__loggers) > 0:
			_logger =self.__loggers[0]
			self.__loggers.remove(_logger)
			del _logger
			_logger = None
		if self.__xmlhandler  :
			self.__xmlhandler.close()
		self.__xmlhandler = None
		return


	def SetLevel(self,level=WARNING_LEVEL):
		l = level
		for _log in self.__loggers:
			l = _log.SetLevel(level)
		return l
	def SetOutput(self,output=1):
		o = output
		for _log in self.__loggers:
			o = _log.SetOutput(output)
		return o
	def Info(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.Info(msg)
		return ret
	def Warn(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.Warn(msg)
		return ret
	def Error(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.Error(msg)
		return ret
	def Debug(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.Debug(msg)
		return ret
	def Flush(self):
		ret = None
		for _log in self.__loggers:
			ret = _log.Flush()
		return ret
	def TestStart(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.TestStart(msg)
		return ret
	def CaseStart(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseStart(msg)
		return ret
	def CaseFail(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseFail(msg)
		return ret
	def CaseError(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseError(msg)
		return ret
	def CaseSucc(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseSucc(msg)
		return ret
	def CaseSkip(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseSkip(msg)
		return ret
	def CaseEnd(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.CaseEnd(msg)
		return ret
	def TestEnd(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.TestEnd(msg)
		return ret
	def write(self,msg):
		ret = None
		for _log in self.__loggers:
			ret = _log.write(msg)
		return ret
	def flush(self):
		ret = None
		for _log in self.__loggers:
			ret = _log.flush()
		return ret



	


_logger_instances = {}

def singleton(class_):
	def get_instance(*args, **kwargs):
		ccn = xunit.utils.cls.GetCallerClassName(2)
 		if ccn not in _logger_instances  :
 			_logger_instances[ccn] = class_(ccn,*args, **kwargs)
			#DebugString('caller %s %s\n'%(ccn,repr(_logger_instances[ccn])))
 		return _logger_instances[ccn]
	return get_instance


def singletonbyargs(class_):
	def getinstance(*args, **kwargs):
		pn = args[0]
		tn =  pn
		if tn not in _logger_instances:			
			_logger_instances[tn] = class_(*args, **kwargs)
			#DebugString('caller %s %s\n'%(tn,repr(_logger_instances[tn])))
		return _logger_instances[tn]
	return getinstance	
	

def logger_cleanup():
	while len(_logger_instances.keys()) > 0:
		k = _logger_instances.keys()[0]
		log1 = _logger_instances[k]
		log1.flush()
		del log1
		log1 = None
		del _logger_instances[k]
	if _AdvLogger.default_xmlhandler :
		_AdvLogger.default_xmlhandler.close()
		del _AdvLogger.default_xmlhandler
	_AdvLogger.default_xmlhandler = None
	_AdvLogger.default_xmllog = None		
	return




@singleton
class AdvLogger(_AdvLogger):
	pass

@singletonbyargs
class ClassLogger(_AdvLogger):
	pass

atexit.register(logger_cleanup)
