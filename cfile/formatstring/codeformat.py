#! python

import sys


def GetBinary(num,fl):
	s = bin(num)
	s = s.replace('0b','')
	# the format is 0bxxx so it will replace
	if len(s) < fl:
		ll = fl - len(s)
		a0 = '0' * ll
		av = a0 + s
		s = av
	return s


def CaseCodeShiftFormat(num,nshift):
	bstr = GetBinary(num,nshift)
	str = '\tcase '
	str += 'MLL_%s:\n'%(bstr)
	str += '\t\tret = format_string(&pResult,&resultnum,args->fmt'
	i = 0
	while i < nshift:
		if bstr[i] == '0':
			str += ',vv[%d]'%(i)
		else:
			str += ',ullv[%d]'%(i)
		i += 1
	str += ');\n'
	str += '\t\tbreak;\n'
	return str

def CaseCodeFormat(nshift):
	maxnum = (1 << nshift)
	i = 0
	str = ''
	while i < maxnum:
		str += CaseCodeShiftFormat(i,nshift)
		i += 1
	return str

def FormatCode(nshift):
	if nshift < 1:
		return ''
	str = 'static int test%d_format(test_args_v_t* args)\n'%(nshift)
	str += '{\n'
	str += '\tchar* pResult=NULL;\n'
	str += '\tint resultnum=0;\n\tint ret=0;\n\tassert(args.numargs == %d);\n'%(nshift)
	str += '\tint setll=0;\n'
	str += '\tint i=0;\n'
	str += '\tvoid *vv[%d];\n'%(nshift)
	str += '\tunsigned long long ull[%d];\n'%(nshift)
	str += '''	for (i=0;i<args->numargs;i++)
	{
		args_v_t pv = args->args;

		switch(pv[i].type)
		{
		case INT_VALUE:
			vv[i] = (void*)pv[i].u.intv;
			break;
		case LONG_VALUE:
			vv[i] = (void*)pv[i].u.longv;
			break;
		case U_LONG_VALUE:
			vv[i] = (void*)pv[i].u.ulongv;
			break;
		case U_INT_VALUE:
			vv[i] = (void*)pv[i].u.uintv;
			break;
		case STR_VALUE:
			vv[i] = (void*)pv[i].u.strv;
			break;
		case CHAR_VALUE:
			vv[i] = (void*)pv[i].u.charv;
			break;
		case U_CHAR_VALUE:
			vv[i] = (void*)pv[i].u.ucharv;
			break;			
		case LL_VALUE:
			ullv[i] = (unsigned long long)pv[i].u.llv;
			setll |= (1 << i);
			break;
		case U_LL_VALUE:
			ullv[i] = (unsigned long long)pv[i].u.ullv;
			setll |= (1 << i);
			break;
		default:
			assert(0!=0);
			break;
		}
	}
'''

	str += '\n\n'
	str += '\tswitch(setll)\n'
	str += '\t{\n'
	str += CaseCodeFormat(nshift)
	str += '\tdefault:\n'
	str += '\t\tbreak;\n'
	str += '\t}\n'
	str += '\n\n'
	str += '''	if (ret < 0)
	{
		goto out;
	}

	ret = 0;
	if (strcmp(pResult,args->result)!=0)
	{
		fprintf(stderr,"compare (%s) != (%s)\\n",pResult,args->result);
		ret = -1;
	}
out:
	free(pResult);
	resultnum = 0;
	return ret;
	
}'''
	return str

def FactoryCodeCaseGen(num):	
	str = '\tcase %d:\n'%(num)
	str += '\t\tret=test%d_format(args);\n'%(num)
	str += '\t\tbreak;\n'
	return str
	

def FactoryCodeGen(nshift):
	str = 'static int FactoryFunc(test_args_v_t* args)\n'
	str += '{\n'
	str += '\tint num=args->numargs,ret=0;\n'
	str += '\tif (num >= %d)\n'%(nshift)
	str += '\t{\n'
	str += '\t\tfprintf(stderr,"args more than %d\\n");\n'%(nshift)
	str += '\t\treturn -EINVAL;\n'
	str += '\t}\n'
	str += '\tswitch(num)\n'
	str += '\t{\n'
	for i in xrange(1,nshift):
		str += FactoryCodeCaseGen(i)
	str += '\tdefault:\n'
	str += '\t\tassert(0!=0);\n'
	str += '\t}\n'
	str += '\n\n'
	str += '}\n'
	return str

if __name__ == '__main__':
	maxnum = int(sys.argv[1])
	for i in xrange(maxnum):
		s = FormatCode(i)
		print '/* format for %d args*/'%(i)
		print s
		print '\n\n'
		i += 1
	s = FactoryCodeGen(maxnum)
	print s
	
