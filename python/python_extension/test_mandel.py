#!/usr/bin/env python

import ctypes
import sys
import mandelbrot

x = -2
y = -1
w = 2.5
h = 2.0

NY = 40
NX = 70
RANGE_Y = range(NY)
RANGE_X = range(NX)

def render(iterate_point):
	chars = []
	append = chars.append
	for j in RANGE_Y:
		for i in RANGE_X:
			it = iterate_point(x+w/NX*i, y+h/NY*j, 1000)
			if it == 1000:
				append("*")
			elif it > 5:
				append(",")
			elif it > 2:
				append(".")
			else:
				append(" ")
		append("\n")
	return "".join(chars)

import time
t1 = time.time()
s1 = render(mandelbrot.iterate_point)
t2 = time.time()
s2 = render(mandelbrot.iterate_point)
t3 = time.time()
assert s1 == s2
print s1
print "as C extension", t2-t1
print "with ctypes", t3-t2
