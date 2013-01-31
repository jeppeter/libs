#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "mansrtsp.h"

static char st_buf[1024];
int ParseAndDump(char* file)
{
	int ret;
	int fd=-1;
	struct mans_rtsp* pMans=NULL;
	char *pDumpBuf=NULL;
	int dumplen=0;
	int dumpsize=0;

	errno = 0;
	fd = open(file,O_RDONLY);
	if (fd < 0)
	{
		ret = -errno ? -errno : -EIO;
		goto out;
	}

	ret = read(fd,st_buf,sizeof(st_buf));
	if (ret < 0)
	{
		ret = -errno ? -errno : -EIO;
		goto out;
	}

	pMans = mans_parse(st_buf);
	if (pMans == NULL)
	{
		ret = -EINVAL;
		goto out;
	}

	dumplen = dump_mans_rtsp(pMans,&pDumpBuf,&dumpsize);
	if (dumplen < 0)
	{
		ret = dumplen;
		goto out;
	}

	fprintf(stdout,"%s",pDumpBuf);
	ret = 0;

	
out:
	if (fd >= 0)
	{
		close(fd);
	}
	mans_destroy(pMans);
	if (pDumpBuf)
	{
		free(pDumpBuf);
	}
	pDumpBuf = NULL;
	dumplen = 0;
	dumpsize = 0;
	return ret;
}

int main(int argc,char* argv[])
{
	int ret;
	int i;

	if (argc < 2)
	{
		fprintf(stderr,"%s files ...\n",argv[0]);
		return -3;
	}

	for (i=1;i<argc;i++)
	{
		ret = ParseAndDump(argv[i]);
		if (ret < 0)
		{
			return ret;
		}
	}

	return 0;
}
