#! python

import unittest
import logging
import xunit.config
from xunit.utils import cls
import xunit.logger
import time
import traceback
from xunit.utils import exception

class FailFastException(exception.XUnitException):
	pass


class XUnitResultBase(unittest.runner.TextTestResult):
	def __init__(self,output=1):
		self.__fails = 0
		self.__succs = 0
		self.__skips = 0
		self.__cases = 0
		self.__unexpectfails = 0
		self.__unexpectsuccs = 0
		self.__output = output
		self.showAll = 0
		self.dots = 0
		self.descriptions = False
		self.shouldStop = False
		super(unittest.runner.TextTestResult,self).__init__(None,'',0)
		utcfg = xunit.config.XUnitConfig()
		v = utcfg.GetValue('global','debug.mode','')
		self.__logger = xunit.logger.ClassLogger('__main__')
		self.__logoutput = self.__logger.SetOutput(self.__output)
		self.__verbose = 0
		self.__resultout = 0
		if v == 'y':
			self.__verbose = 1
		self.__failfast = 0
		v = utcfg.GetValue('global','failfast','')
		if v == 'y':
			self.__failfast = 1
		self.__logger.TestStart('start :')
		self.__ps1 = '=' * 60
		self.__ps2 = '-' * 60
		return

	def RestoreLogOutput(self):
		if self.__logger:
			self.__logger.SetOutput(self.__logoutput)

	def __printErrorslist(self,note,errors):
		for test, err in errors:
			self.__logger.write(self.__ps1+'\n')
			self.__logger.write("%s: %s\n" % (note,test))
			self.__logger.write(self.__ps2+'\n')
			self.__logger.write("%s\n" % err)		
		return

	def __printErros(self):
		self.__printErrorslist('ERROR',self.errors)
		self.__printErrorslist('FAIL',self.failures)
		self.__logger.flush()
		return
		
	def ResultAll(self):
		if self.__resultout :
			return
		if self.__logger and self.__output:
			self.__printErros()
		if self.__logger and self.__output:
			self.__logger.TestEnd('Cases %d Succ %d Fail %d Skip %d'%(self.__cases,self.__succs,self.__fails,self.__skips))
		self.RestoreLogOutput()
		self.__resultout = 1

	def __del__(self):
		self.ResultAll()

	def _exc_info_to_string(self, err, test):
		"""Converts a sys.exc_info()-style tuple of values into a string."""
		exctype, value, tb = err
		# Skip test runner traceback levels
		while tb and self._is_relevant_tb_level(tb):
			tb = tb.tb_next

		if exctype is test.failureException:
			# Skip assert*() traceback levels
			length = self._count_relevant_tb_levels(tb)
			msgLines = traceback.format_exception(exctype, value, tb, length)
		else:
			msgLines = traceback.format_exception(exctype, value, tb)		
		return ''.join(msgLines)

	def _is_relevant_tb_level(self, tb):
		return '__unittest' in tb.tb_frame.f_globals

	def _count_relevant_tb_levels(self, tb):
		length = 0
		while tb and not self._is_relevant_tb_level(tb):
			length += 1
			tb = tb.tb_next
		return length


	def startTest(self, test):
		if self.__verbose and self.__output > 0:
			cn = cls.GetClassName(test.__class__)
			self.__logger.CaseStart(cn+':'+test._testMethodName)
		return

	def addSuccess(self, test):
		self.__succs += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseSucc('Success')
			self.__logger.CaseEnd('')
		return

	def addError(self, test, err):
		self.__fails += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseError('Error')
			self.__logger.CaseEnd('')
		if self.__failfast:
			self.shouldStop = True
		self.errors.append((test, self._exc_info_to_string(err, test)))
		return

	def addFailure(self, test, err):
		self.__fails += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseFail('Failure')
			self.__logger.CaseEnd('')
		if self.__failfast:
			self.shouldStop = True
		self.failures.append((test, self._exc_info_to_string(err, test)))
		return
	

	def addSkip(self, test, reason):
		self.__skips += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseSkip('Skip')
			self.__logger.CaseEnd('')
		return

	def addExpectedFailure(self, test, err):
		self.__unexpectfails += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseFail('Expected Failure')
			self.__logger.CaseEnd('')
		if self.__failfast:
			self.shouldStop = True
		return

	def addUnexpectedSuccess(self, test):
		self.__unexpectsuccs += 1
		self.__cases += 1
		if self.__verbose and self.__output > 0:
			self.__logger.CaseFail('Unexpected Success')
			self.__logger.CaseEnd('')
		if self.__failfast:
			self.shouldStop = True
		return

	def Cases(self):
		return self.__cases
	def Succs(self):
		return self.__succs

	def Fails(self):
		return self.__fails

	def Skips(self):
		return self.__skips

	def UnexpectFails(self):
		return self.__unexpectfails

	def UnexpectSuccs(self):
		return self.__unexpectsuccs

	def wasSuccessful(self):
		return self.__fails == 0 and self.__unexpectfails == 0 and self.__unexpectsuccs == 0

def singleton(cls):
	instances = {}
	def get_instance():
		if cls not in instances:
			instances[cls] = cls()
		return instances[cls]
	return get_instance

@singleton
class XUnitResult(XUnitResultBase):
	pass

