// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\\dllcri\\dllcri.h"

#pragma comment(lib,"..\\Debug\\dllcri.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	printf("SnapWholePicture file %d\n",SnapWholePicture("z:\\bitmap.bmp"));
	return 0;
}

