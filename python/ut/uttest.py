#! python

import UTConfig
import unittest
import sys
import logging

class UtTest(unittest.TestCase):
	def test_LoadBasic(self):
		# now for 
		utcfg = UTConfig.UTConfig('base.cfg')
		logging.info('\n')
		self.assertEqual(utcfg.GetValue('new','base2'),'hello world')
		logging.info('\n')
		return



if __name__ == '__main__':
	if '-v' in sys.argv[1:] or '--verbose' in sys.argv[1:]:
		sys.stderr.write('-v in\n')
		logging.basicConfig(level=logging.INFO,format="%(levelname)-8s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
	unittest.main()

