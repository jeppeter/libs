#! python

import UTConfig
import unittest
import sys
import logging

class UtTest(unittest.TestCase):
	def test_LoadBasic(self):
		# now for 
		utcfg = UTConfig.UTConfigBase('base.cfg')
		self.assertEqual(utcfg.GetValue('new','base2'),'hello world')
		return

	def test_IncludeFiles(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		includes = utcfg.GetIncludeFiles()
		self.assertTrue( 'inc.cfg' in includes )
		self.assertTrue( 'base.cfg' in includes )
		#self.assertTrue(len(includes) == 2)
		return

	def test_Unittests(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		units = utcfg.GetUnitTests()
		self.assertTrue('base.unit.test' in units)
		self.assertTrue('inc.unit.test' in units)
		self.assertTrue(len(units) == 2)
		return

	def test_SearchPaths(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		paths = utcfg.GetSearchPaths()
		self.assertTrue(len(paths) == 1)
		self.assertTrue( 'inc' in paths)
		return

	def test_OverflowError(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
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
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		ok = 1
		try:
			v = utcfg.GetValue('valuebase','base1')
		except UTConfig.UTCfgOverflowError as e:
			ok = 0
		self.assertTrue( ok == 0)
		return

	def test_NonValue(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		v = utcfg.GetValue('no_section','no_opt')
		self.assertTrue( v == '')
		return
		
	def test_3LevelRef(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		v = utcfg.GetValue('base2.value','base2')
		self.assertEqual(v,'hello param1')
		return

	def test_noref(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		v = utcfg.GetValue('base2.value','base3')
		self.assertEqual(v,' param1')
		return

	def test_setgetvalue(self):
		utcfg = UTConfig.UTConfigBase('inc.cfg')
		utcfg.SetValue('base2.value','base4','%(base2.value.base2)s value')
		v = utcfg.GetValue('base2.value','base4')
		self.assertEqual(v,'hello param1 value')
		return

	def test_LoadError(self):
		ok = 1
		try:
			utcfg = UTConfig.UTConfigBase()
			utcfg.LoadFile('nocfg.cfg')
		except UTConfig.UTCfgLoadFileError as e:
			ok = 0
		self.assertTrue(ok == 0)
		return

	def test_SearchPathExp(self):
		utcfg = UTConfig.UTConfigBase()
		utcfg.LoadFile('exp.cfg')
		paths = utcfg.GetSearchPaths()
		self.assertTrue( '/usr/inc' in paths )
		return

	def test_SearchPathNoRef(self):
		utcfg = UTConfig.UTConfigBase()
		utcfg.LoadFile('exp.cfg')
		paths = utcfg.GetSearchPaths()
		self.assertTrue( '/nopath' in paths )
		return

	def test_Singleton(self):
		utcfg = UTConfig.UTConfig()
		ut2 = UTConfig.UTConfig()
		self.assertTrue( utcfg == ut2)
		utcfg.LoadFile('exp.cfg')
		paths = utcfg.GetSearchPaths()
		self.assertTrue( '/nopath' in paths )
		ut2.LoadFile('inc.cfg')
		paths = ut2.GetSearchPaths()
		self.assertTrue( '/nopath' in paths )
		ut2.SetValue('no_section','no_opt','no_value')
		v = utcfg.GetValue('no_section','no_opt')
		self.assertEqual(v,'no_value')
		return

		
if __name__ == '__main__':
	if '-v' in sys.argv[1:] or '--verbose' in sys.argv[1:]:
		logging.basicConfig(level=logging.INFO,format="%(levelname)-8s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
	unittest.main()

