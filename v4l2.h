#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             
#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include<linux/videodev2.h>


#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        unsigned int  length;
};

typedef struct controls
{
	int ctrlcount, index;
	struct v4l2_queryctrl uqueryctrl;
	struct v4l2_control ucontrol;
}CONTROLS;

