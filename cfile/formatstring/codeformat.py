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


def CaseCodeFloatShiftFormat(ntabs,numll,numff,nshift):
	fstr = GetBinary(numff,nshift)
	bstr = GetBinary(numll,nshift)
	str = ' ' * (ntabs * 4)	
	str += 'case MFF_%s:\n'%(fstr)
	str += ' ' * (ntabs + 1)*4
	str += 'fprintf(stderr,"[%%s:%%d]\tUSE setll MLL_%s setff MFF_%s\\n",__FILE__,__LINE__);\n'%(bstr,fstr)
	str += ' ' * (ntabs + 1)*4
	str += 'ret = format_string(&pResult,&resultnum,args->fmt'
	for i in xrange(0,nshift):
		j = nshift - i - 1
		# we use this for the value
		if bstr[j] == '1':
			str += ',ullv[%d]'%(i)
		elif fstr[j] == '1':
			str += ',fv[%d]'%(i)
		else:
			str += ',vv[%d]'%(i)
	str += ');\n'
	str += ' ' * (ntabs + 1)*4
	str += 'if (ret < 0)\n'
	str += ' ' * (ntabs + 1)*4
	str += '{\n'
	str += ' ' * (ntabs + 2)*4
	str += 'goto out;\n'
	str += ' ' * (ntabs + 1)*4
	str += '}\n'
	str += ' ' * (ntabs + 1)*4
	str += 'break;\n'
	return str

def CaseCodeFloatFormat(ntabs,numll,nshift):
	str = ' ' * (ntabs * 4)
	str += 'switch(setff)\n'
	str += ' ' * (ntabs * 4)
	str += '{\n'
	maxnum = (1 << nshift)
	for i in xrange(0,maxnum):
		fstr = GetBinary(i,nshift)
		bstr = GetBinary(numll,nshift)
		usecase = 1
		for j in xrange(0,nshift):
			if fstr[j] == '1' and bstr[j] =='1':
				#if we have the both set ,it can not set,so omit it
				usecase = 0
				break
		if usecase == 1:
			str += CaseCodeFloatShiftFormat(ntabs + 1,numll,i,nshift)

	str += ' '*(ntabs+1)*4
	str += 'default:\n'
	str += ' '*(ntabs + 2)*4
	str += 'assert(0!=0);\n'
	str += ' '*(ntabs + 2)*4
	str += 'break;\n'
	str += ' ' * (ntabs)*4
	str += '}\n'
	return str

def CaseCodeShiftFormat(ntabs,num,nshift):
	bstr = GetBinary(num,nshift)
	str = ' ' * ntabs*4
	str += 'case MLL_%s:\n'%(bstr)
	str += CaseCodeFloatFormat(ntabs+1,num,nshift)
	str += ' ' * (ntabs+1)*4
	str += 'break;\n'
	return str

def CaseCodeFormat(ntabs,nshift):
	maxnum = (1 << nshift)
	i = 0
	str = ''
	for i in xrange(0,maxnum):
		str += CaseCodeShiftFormat(ntabs,i,nshift)
	return str

def FormatCode(nshift):
	ntabs = 1
	if nshift < 1:
		str = '''static int test0_format(test_args_v_t* args)
{
	char* pResult=NULL;
	int resultnum=0;
	int ret;
	ret = format_string(&pResult,&resultnum,args->fmt);
	if (ret < 0)
	{
		goto out;
	}

	ret = 0;
	if (strcmp(pResult,args->result)!=0)
	{
		ret = -EINVAL;
		fprintf(stderr,"result (%s) not valid (%s)\\n",pResult,args->result);
	}
out:
	if (pResult)
	{
		free(pResult);
	}
	pResult = NULL;
	resultnum = 0;
	return ret;
}'''
		return str
	str = 'static int test%d_format(test_args_v_t* args)\n'%(nshift)
	str += '{\n'
	str += ' ' * ntabs*4
	str += 'char* pResult=NULL;\n'
	str += ' ' * ntabs*4
	str += 'int resultnum=0;\n'
	str += ' ' * ntabs*4
	str += 'int ret=0;\n'
	str += ' ' * ntabs*4
	str += 'assert(args->numargs == %d);\n'%(nshift)
	str += ' ' * ntabs*4
	str += 'int setll=0;\n'
	str += ' ' * ntabs*4
	str += 'int setff=0;\n'
	str += ' ' * ntabs*4
	str += 'int i=0;\n'
	str += ' ' * ntabs*4
	str += 'unsigned long vv[%d];\n'%(nshift)
	str += ' ' * ntabs*4
	str += 'unsigned long long ullv[%d];\n'%(nshift)
	str += ' ' * ntabs*4
	str += 'float fv[%d];\n'%(nshift)
	str += '''	for (i=0;i<args->numargs;i++)
	{
		args_v_t *pv = args->args;

		switch(pv[i].type)
		{
		case INT_VALUE:
			vv[i] = (unsigned long)pv[i].u.intv;
			break;
		case LONG_VALUE:
			vv[i] = (unsigned long)pv[i].u.longv;
			break;
		case U_LONG_VALUE:
			vv[i] = (unsigned long)pv[i].u.ulongv;
			break;
		case U_INT_VALUE:
			vv[i] = (unsigned long)pv[i].u.uintv;
			break;
		case STR_VALUE:
			vv[i] = (unsigned long)pv[i].u.strv;
			break;
		case CHAR_VALUE:
			vv[i] = (unsigned long)pv[i].u.charv;
			break;
		case U_CHAR_VALUE:
			vv[i] = (unsigned long)pv[i].u.ucharv;
			break;			
		case LL_VALUE:
			ullv[i] = (unsigned long long)pv[i].u.llv;
			setll |= (1 << i);
			break;
		case U_LL_VALUE:
			ullv[i] = (unsigned long long)pv[i].u.ullv;
			setll |= (1 << i);
			break;
		case FLOAT_VALUE:
			fv[i] =  pv[i].u.fv;
			setff |= (1 << i);			
			break;
		case DOUBLE_VALUE:
			fv[i] = pv[i].u.dv;
			setff |= (1 << i);
			break;
		default:
			assert(0!=0);
			break;
		}
	}
'''

	str += '\n\n'
	str += ' ' * ntabs*4
	str += 'switch(setll)\n'
	str += ' ' * ntabs*4
	str += '{\n'
	str += CaseCodeFormat(ntabs + 1,nshift)
	str += ' ' * ntabs*4
	str += 'default:\n'
	str += ' ' * (ntabs + 1)*4
	str += 'assert(0!=0);\n'
	str += ' ' * (ntabs+1)*4
	str += 'break;\n'
	str += ' ' * ntabs*4
	str += '}\n'
	str += '\n\n'
	str += '''	if (ret < 0)
	{
		goto out;
	}

	ret = 0;
	if (strcmp(pResult,args->result)!=0)
	{
		fprintf(stderr,"[%s:%d]compare (%s) != (%s)\\n",__FILE__,__LINE__,pResult,args->result);
		ret = -1;
	}
out:
	free(pResult);
	resultnum = 0;
	return ret;
	
}'''
	return str

def FactoryCodeCaseGen(ntabs,num):	
	str = ' ' * ntabs*4
	str += 'case %d:\n'%(num)
	str += ' ' * (ntabs+1)*4
	str += 'ret=test%d_format(args);\n'%(num)
	str += ' ' * (ntabs+1)*4
	str += 'break;\n'
	return str
	

def FactoryCodeGen(nshift):
	ntabs = 1
	str = 'static int FactoryFunc(test_args_v_t* args)\n'
	str += '{\n'
	str += ' ' * ntabs*4
	str += 'int num=args->numargs,ret=0;\n'
	str += ' ' * ntabs*4
	str += 'if (num >= %d)\n'%(nshift)
	str += ' ' * ntabs*4
	str += '{\n'
	str += ' ' * (ntabs+1)*4
	str += 'fprintf(stderr,"args more than %d\\n");\n'%(nshift)
	str += ' ' * (ntabs+1)*4
	str += 'return -EINVAL;\n'
	str += ' ' * ntabs*4
	str += '}\n'
	str += ' ' * ntabs*4
	str += 'switch(num)\n'
	str += ' ' * ntabs*4
	str += '{\n'
	for i in xrange(0,nshift):
		str += FactoryCodeCaseGen(ntabs + 1,i)
	str += ' ' * (ntabs+1)*4
	str += 'default:\n'
	str += ' ' * (ntabs+2)*4
	str += 'assert(0!=0);\n'
	str += ' ' * ntabs*4
	str += '}\n'
	str += '\n\n'
	str += ' ' * ntabs* 4
	str += 'return ret;\n'
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
	
