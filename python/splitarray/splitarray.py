#! python

import fileinput,sys

def GetArray(file):
	a = []
	for l in fileinput.input(file):
		l.rstrip("\r\n")
		num = int(l)
		a.append(num)
	return a

def GetTwoArray(file1,file2):
	a = GetArray(file1)
	b = GetArray(file2)
	return a,b

def SplitTwoArrayEven(a,b):
	asort = sorted(a)
	bsort = sorted(b)
	total = 0
	atotal = 0
	btotal = 0
	for num in bsort:
		btotal += num
	for num in asort:
		atotal += num
	total = atotal + btotal
	needone = total / 2
	
if __name__ == '__main__':
	a,b = GetTwoArray(sys.argv[1],sys.argv[2])
	SplitTwoArrayEven(a,b)