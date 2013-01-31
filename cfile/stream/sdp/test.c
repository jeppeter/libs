#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "sdp.h"
#include <stdlib.h>


int ParseAndDump(const char* file)
{
    int fd = open(file, O_RDONLY);
    struct sdp_payload *sdp;
    char payload[1024+1];
	char *pString=NULL;
	int stringlen =0;
    int n;

    if(fd < 0)
    {
        perror("open");
        return -1;
    }

    n = read(fd, payload, sizeof(payload));
    if(n < 0)
    {
        perror("read");
        close(fd);
        return -1;
    }
    close(fd);
    payload[n] = 0;
    sdp = sdp_parse(payload);
    if(sdp == NULL)
    {
        perror("parse sdp");
        return -1;
    }
    printf("[%s]\n", file);
    n=sdp_dump(sdp,&pString,&stringlen);
	if (n >= 0)
	{
		printf("%s\n",pString);				
	}
    sdp_destroy(sdp);
	if (pString)
	{
		free(pString);
	}
	pString = NULL;
	stringlen = 0;
    return n >= 0 ? 0 : n;
}

int main(int argc,char* argv[])
{
    size_t i;
    int ret;

    if(argc <=1)
    {
		fprintf(stderr,"%s files...\n",argv[0]);
    }
	else
	{
		for(i=1; i<argc; i++)
		{
			ret = ParseAndDump(argv[i]);
			if(ret < 0)
			{
				return ret;
			}
		}
	}
	return 0;
}
