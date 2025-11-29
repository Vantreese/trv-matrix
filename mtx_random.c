#include <stdio.h>
#include <stdlib.h>

//#include <fcntl.h>      // for open()
//#include <unistd.h>     // for close(), read()

unsigned char *mtx_random(void *v, int rndsize)
{
	FILE *rnddev;
	//int fhandle;	// for open()
	unsigned char *rndbuf = (unsigned char*) v;
	unsigned int *ivalue = (unsigned int*) v;
	char commandline[80];

	if (rndsize <= 0) return NULL;
	if (!rndbuf) rndbuf = (unsigned char*) malloc(rndsize);

	// fopen()/popen() method
	if (!rndbuf) return NULL;
	//rnddev = popen("cat /dev/random", "rb");	// Temporary fix because "/dev/random" won't fopen().
	sprintf(commandline, "randomdevice %d", ((rndsize + 3) / 4));	// Call random, then fix random only.
	rnddev = popen(commandline, "rb");
	if (!rnddev)
	{
		fprintf(stderr, "Can't open randomdevice.\n");
		return NULL;
	}
	fread(rndbuf, rndsize, 1, rnddev);
	fclose(rnddev);

	// raw open() method
	/*
	fhandle = open("/dev/random", O_RDONLY);
	if (fhandle < 0)
	{
		fprintf(stderr, "Can't open /dev/random.\n");
	}
	read(fhandle, rndbuf, rndsize);
	close(fhandle);
	*/

	return rndbuf;
}
