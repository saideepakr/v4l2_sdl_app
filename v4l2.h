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

struct v4l2_ufrmsizeenum
{
	__u32			index;		/* Frame size number */
	__u32			pixel_format;	/* Pixel format */
	__u32			type;		/* Frame size type the device supports. */

	union {					/* Frame size */
		struct v4l2_frmsize_discrete	discrete;
		struct v4l2_frmsize_stepwise	stepwise;
	};

	__u32   reserved[2];			/* Reserved space for future use */
	
	__u32			fpscount;
	struct v4l2_frmivalenum *frmivalenum;
};

struct v4l2_ufmtdesc
{
	__u32		    index;             /* Format number      */
	__u32		    type;              /* enum v4l2_buf_type */
	__u32               flags;
	__u8		    description[32];   /* Description string */
	__u32		    pixelformat;       /* Format fourcc      */
	__u32		    reserved[4];
	
	__u32 			frmcount;
	struct v4l2_ufrmsizeenum *ufrmsizeenum;
};

typedef struct formats
{
	__u32 fmtcount;
	struct v4l2_ufmtdesc *ufmtdesc;
}FORMATS;






