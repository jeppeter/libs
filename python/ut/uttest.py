#! python

import UTConfig
import unittest
import sys
import logging

class UtTest(unittest.TestCase):
	def test_LoadBasic(self):
		# now for 
		utcfg = UTConfig.UTConfig('base.cfg')
		self.assertEqual(utcfg.GetValue('new','base2'),'hello world')
		return

	def test_IncludeFiles(self):
		utcfg = UTConfig.UTConfig('inc.cfg')
		includes = utcfg.GetIncludeFiles()
		self.assertTrue( 'inc.cfg' in includes )
		self.assertTrue( 'base.cfg' in includes )
		self.assertTrue(len(includes) == 2)
		return

	def test_Unittests(self):
		utcfg = UTConfig.UTConfig('inc.cfg')
		units = utcfg.GetUnitTests()
		self.assertTrue('base.unit.test' in units)
		self.assertTrue('inc.unit.test' in units)
		self.assertTrue(len(units) == 2)
		return

	def test_SearchPaths(self):
		utcfg = UTConfig.UTConfig('inc.cfg')
		paths = utcfg.GetSearchPaths()
		self.assertTrue(len(paths) == 1)
		self.assertTrue( '/usr/inc' in paths)
		return

	def test_OverflowError(self):
		utcfg = UTConfig.UTConfig('inc.cfg')
		ok = 1
		try:
			v = utcfg.GetValue('value1','base1')
		except UTConfig.UTCfgOverflowError as e:
			ok = 0
		self.assertTrue( ok == 0)

		ok = 1
		try:
			v = utcfg.GetValue('value2','base2')
		except UTConfig.UTCfgOverflowError as e:
			ok = 0
		self.assertTrue( ok == 0)
		return

	def test_SectionInnerOverflowError(self):
		utcfg = UTConfig.UTConfig('inc.cfg')
		ok = 1
		try:
			v = utcfg.GetValue('valuebase','base1')
		except UTConfig.UTCfgOverflowError as e:
			ok = 0
		self.assertTrue( ok == 0)
		return

if __name__ == '__main__':
	if '-v' in sys.argv[1:] or '--verbose' in sys.argv[1:]:
		logging.basicConfig(level=logging.INFO,format="%(levelname)-8s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
	unittest.main()

