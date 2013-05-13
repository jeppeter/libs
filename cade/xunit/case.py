#! python

import unittest
import logging
from xunit.utils import cls

class XUnitCase(unittest.TestCase):
	@classmethod
	def XUnitsetUpClass(cls):
		return

	@classmethod
	def XUnittearDownClass(cls):
		return

	@classmethod
	def setUpClass(cls):
		cls.XUnitsetUpClass()
		return


	@classmethod
	def tearDownClass(cls):
		cls.XUnittearDownClass()
		return

	def XUnitsetUp(self):
		return

	def XUnittearDown(self):
		return

	def setUp(self):
		self.XUnitsetUp()
		return

	def tearDown(self):
		self.XUnittearDown()
		return

