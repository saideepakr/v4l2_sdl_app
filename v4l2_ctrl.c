#include "v4l2.h"
#include "v4l2_ctrl.h"
#include "capture.h"

/**
Function Name : deviceInfo
Function Description : Displays the information about the device
Parameter : void
Return : void
**/
void deviceInfo(void)
{
	struct v4l2_capability cap;
	int index;
	
	 if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) 
	 {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\\n",
                                 dev_path);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
     }
     printf("Driver Info\n");
     printf("\tDriver Name : %s\n", cap.driver);
     printf("\tCard Type : %s\n", cap.card);
     printf("\tBus info : %s\n", cap.bus_info);
     printf("\tDriver Version : %u\n", cap.version);
     printf("\tCapabilities : %#x\n", cap.capabilities);
     
     if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
     	printf("\t\tVideo Capture\n");
     if(cap.capabilities & V4L2_CAP_STREAMING)
     	printf("\t\tStreaming\n");
     if(cap.capabilities & V4L2_CAP_EXT_PIX_FORMAT)
     	printf("\t\tExtended Pix Format\n");
     if(cap.capabilities & V4L2_CAP_DEVICE_CAPS)
     {
     	printf("\tDevice Caps : %#x\n", cap.device_caps);
     	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		 	printf("\t\tVideo Capture\n");
		if(cap.capabilities & V4L2_CAP_STREAMING)
		 	printf("\t\tStreaming\n");
		if(cap.capabilities & V4L2_CAP_EXT_PIX_FORMAT)
		 	printf("\t\tExtended Pix Format\n");
     }
}

/**
Function Name : bufferTypeToString
Function Description : Displays the information about the device
Parameter : unsigned integer type of V4L2 Capabilities
Return : void
**/
void bufferTypeToString(unsigned int ui_type)
{
	if(ui_type == V4L2_CAP_VIDEO_CAPTURE)
		printf("Video Capture");
	else if(ui_type == V4L2_CAP_VIDEO_OUTPUT)
		printf("Video Output");
}

/**
Function Name : fcc2s
Function Description : Prints the pixel format
Parameter : unsigned integer pixel format
Return : void
**/
void fcc2s(unsigned int ui_pixel_format)
{
	printf("'%c%c%c%c'",(ui_pixel_format & 0xff), ((ui_pixel_format >> 8) & 0xff), ((ui_pixel_format >> 16) & 0xff), ((ui_pixel_format >> 24) & 0xff));
}

/**
Function Name : frmtype2s
Function Description : Uses unsigned integer type and returns the frame type string
Parameter : unsigned integer type
Return : static constant character pointer
**/
static const char* frmtype2s(unsigned type)
{
	static const char *types[] = {
		"Unknown",
		"Discrete",
		"Continuous",
		"Stepwise"
	};

	if (type > 3)
		type = 0;
	return types[type];
}

/**
Function Name : print_frmsize
Function Description : Prints the frame size
Parameter : v4l2_frmsizeenum structure and prefix
Return : void
**/
void print_frmsize(const struct v4l2_frmsizeenum frmsize, const char *prefix)
{
	printf("%s\tSize: %s ", prefix, frmtype2s(frmsize.type));
	if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) 
	{
		printf("%dx%d", frmsize.discrete.width, frmsize.discrete.height);
	} 
	else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) 
	{
		printf("%dx%d - %dx%d with step %d/%d",
				frmsize.stepwise.min_width,
				frmsize.stepwise.min_height,
				frmsize.stepwise.max_width,
				frmsize.stepwise.max_height,
				frmsize.stepwise.step_width,
				frmsize.stepwise.step_height);
	}
	printf("\n");
}

/**
Function Name : fract2sec
Function Description : Converts fractions to seconds and Prints the seconds
Parameter : v4l2_fract structure
Return : void
**/
void fract2sec(const struct v4l2_fract f)
{
	printf("%.3fs ", (1.0 * f.numerator) / f.denominator);
}

/**
Function Name : fract2fps
Function Description : Converts fractions to frames per second and Prints the frames per second
Parameter : v4l2_fract structure
Return : void
**/
void fract2fps(const struct v4l2_fract f)
{
	printf("(%.3f fps)", (1.0 * f.denominator) / f.numerator);
}

/**
Function Name : print_frmival
Function Description : Prints the Frame intervals
Parameter : v4l2_frmivalenum structure and prefix
Return : void
**/
void print_frmival(const struct v4l2_frmivalenum frmival, const char *prefix)
{
	printf("%s\tInterval: %s ", prefix, frmtype2s(frmival.type));
	if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE) 
	{
		fract2sec(frmival.discrete);
		fract2fps(frmival.discrete);
		printf("\n");
	} 
	else if (frmival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) 
	{
		fract2sec(frmival.stepwise.min);	printf("- ");
		fract2sec(frmival.stepwise.max);	printf(" (");
		fract2fps(frmival.stepwise.max);
		fract2fps(frmival.stepwise.min);	printf(")\n");
	}
	else if (frmival.type == V4L2_FRMIVAL_TYPE_STEPWISE) 
	{
		fract2sec(frmival.stepwise.min);	printf("- ");
		fract2sec(frmival.stepwise.max);	printf(" with step ");
		fract2sec(frmival.stepwise.step);	printf(" (");
		fract2fps(frmival.stepwise.max);
		fract2fps(frmival.stepwise.min);	printf(")\n");
	}
}

/**
Function Name : listFormats
Function Description : Prints the available formats
Parameter : void
Return : int
**/
int listFormats(void)
{

	struct v4l2_capability cap;
	
	if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) 
 	{
            if (EINVAL == errno) 
            {
                    fprintf(stderr, "%s is no V4L2 device\\n",
                             dev_path);
                    exit(EXIT_FAILURE);
            } 
            else 
            {
                    errno_exit("VIDIOC_QUERYCAP");
            }
 	}
 
 	if(!cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
 	{
 		printf("\t\tVideo Capture is not supported\n");
 		return -1;
 		
 	}
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	struct v4l2_frmivalenum frmival;
	
	fmt.index = 0;
	fmt.type = type;
	
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
		printf("\tIndex       : %d\n", fmt.index);
		printf("\tType        : ");	bufferTypeToString(fmt.type); printf("\n");
		printf("\tPixel Format: "); fcc2s(fmt.pixelformat);
		if (fmt.flags)
		{
			if(fmt.flags == V4L2_FMT_FLAG_COMPRESSED)
				printf(" (Compressed)");
			else
				printf(" (Emulated)");
		}
		printf("\n");
		printf("\tName        : %s\n", fmt.description);
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
			print_frmsize(frmsize, "\t");
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				frmival.index = 0;
				frmival.pixel_format = fmt.pixelformat;
				frmival.width = frmsize.discrete.width;
				frmival.height = frmsize.discrete.height;
				while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0) {
					print_frmival(frmival, "\t\t");
					frmival.index++;
				}
			}
			frmsize.index++;
		}
		printf("\n");
		fmt.index++;
}
	return 0;
	
}

/**
Function Name : enumerateMenu
Function Description : Enumerates and Prints the available menus in the control
Parameter : v4l2_queryctrl structure
Return : void
**/
void enumerateMenu(struct v4l2_queryctrl queryctrl)
{
	struct v4l2_querymenu querymenu;
	CLEAR(querymenu);
	querymenu.id = queryctrl.id;
	
	printf("\n");
	for (querymenu.index = queryctrl.minimum; querymenu.index <= queryctrl.maximum; querymenu.index++) 
    {
        if (0 != ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) 
        	continue;
        if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
				printf("\t\t\t\t%d: %s\n", querymenu.index, querymenu.name);
		else
				printf("\t\t\t\t%d: %lld (0x%llx)\n", querymenu.index, querymenu.value, querymenu.value);
    }
}

/**
Function Name : listControls
Function Description : Prints the available controls
Parameter : void
Return : void
**/
void listControls(void)
{
	struct v4l2_capability cap;
	
	if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) 
 	{
            if (EINVAL == errno) 
            {
                    fprintf(stderr, "%s is no V4L2 device\\n",
                             dev_path);
                    exit(EXIT_FAILURE);
            } 
            else 
            {
                    errno_exit("VIDIOC_QUERYCAP");
            }
 	}
 
 	if(!cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
 	{
 		printf("\t\tVideo Capture is not supported\n");
 		return;
 		
 	}
 	
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	
	CLEAR(queryctrl);
	
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) 
	{
    	if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) 
    	{
    		printf("\n");
        	switch (queryctrl.type) 
        	{
        		case V4L2_CTRL_TYPE_INTEGER:
					printf("%25s %#8.8x (int)    : min=%d max=%d step=%d default=%d",
					queryctrl.name, queryctrl.id,
					queryctrl.minimum, queryctrl.maximum,
					queryctrl.step, queryctrl.default_value);
				break;
				
				case V4L2_CTRL_TYPE_INTEGER64:
					printf("%25s %#8.8x (int64)  : min=%d max=%d step=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.step, queryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_STRING:
					printf("%25s %#8.8x (str)    : min=%d max=%d step=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.step);
					break;
				case V4L2_CTRL_TYPE_BOOLEAN:
					printf("%25s %#8.8x (bool)   : default=%d",
							queryctrl.name, queryctrl.id, queryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_MENU:
					printf("%25s %#8.8x (menu)   : min=%d max=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.default_value);
					enumerateMenu(queryctrl);
					break;
				case V4L2_CTRL_TYPE_INTEGER_MENU:
					printf("%25s %#8.8x (intmenu): min=%d max=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.default_value);
					enumerateMenu(queryctrl);
					break;
				case V4L2_CTRL_TYPE_BUTTON:
					printf("%25s %#8.8x (button) :", queryctrl.name, queryctrl.id);
					break;
				case V4L2_CTRL_TYPE_BITMASK:
					printf("%25s %#8.8x (bitmask): max=0x%08x default=0x%08x",
							queryctrl.name, queryctrl.id, queryctrl.maximum,
							queryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U8:
					printf("%25s %#8.8x (u8)     : min=%d max=%d step=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.step, queryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U16:
					printf("%25s %#8.8x (u16)    : min=%d max=%d step=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.step, queryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U32:
					printf("%25s %#8.8x (u32)    : min=%d max=%d step=%d default=%d",
							queryctrl.name, queryctrl.id,
							queryctrl.minimum, queryctrl.maximum,
							queryctrl.step, queryctrl.default_value);
					break;
				default:
					printf("%25s %#8.8x (unknown): type=%x",
							queryctrl.name, queryctrl.id, queryctrl.type);
					break;

							
        	}
        	
        	CLEAR(control);
			control.id = queryctrl.id;

			if (0 == ioctl(fd, VIDIOC_G_CTRL, &control)) 
			{
				printf(" value=%d", control.value);
			}
			
        }
        
        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
     }
     printf("\n");
}

/**
Function Name : loadControls
Function Description : Stores the available controls in structure
Parameter : void
Return : void pointer
**/
void* loadControls(void)
{
	int count = 0, idx = 1;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	CLEAR(queryctrl);
	
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	
	while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) 
	{
    	if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) 
    		count++;
    	
        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    
    if((pcontrol = (CONTROLS*)malloc(sizeof(CONTROLS) * count)) == NULL)
    {
    	printf("\nMalloc Fails");
    	return NULL;
    }
    pcontrol->ctrlcount = count;
    //printf("%d %d", pcontrol->ctrlcount, count);
    CLEAR(queryctrl);
	
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) 
	{
    	if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) 
    	{
    		(pcontrol + idx - 1)->index = idx;
    		(pcontrol + idx - 1)->uqueryctrl.id = queryctrl.id;
    		strcpy((pcontrol + idx - 1)->uqueryctrl.name, queryctrl.name);
    		(pcontrol + idx - 1)->uqueryctrl.type = queryctrl.type;
    		switch (queryctrl.type) 
        	{
        		case V4L2_CTRL_TYPE_INTEGER:
					//printf("%25s %#8.8x (int)    : min=%d max=%d step=%d default=%d",
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;
				break;
				
				case V4L2_CTRL_TYPE_INTEGER64:
					//printf("%25s %#8.8x (int64)  : min=%d max=%d step=%d default=%d",
							//queryctrl.name, queryctrl.id,
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;		
					break;
					
				case V4L2_CTRL_TYPE_STRING:
					//printf("%25s %#8.8x (str)    : min=%d max=%d step=%d",
						//	queryctrl.name, queryctrl.id,
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					break;
				case V4L2_CTRL_TYPE_BOOLEAN:
					//printf("%25s %#8.8x (bool)   : default=%d",
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;
					(pcontrol + idx - 1)->uqueryctrl.minimum = 0;
					(pcontrol + idx - 1)->uqueryctrl.maximum = 1;	
					break;
				case V4L2_CTRL_TYPE_MENU:
					//printf("%25s %#8.8x (menu)   : min=%d max=%d default=%d",
						//	queryctrl.name, queryctrl.id,
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;
					//loadMenu(queryctrl);
					break;
				case V4L2_CTRL_TYPE_INTEGER_MENU:
					//printf("%25s %#8.8x (intmenu): min=%d max=%d default=%d",
						//	queryctrl.name, queryctrl.id,
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;
					//loadMenu(queryctrl);
					break;
				case V4L2_CTRL_TYPE_BUTTON:
					//printf("%25s %#8.8x (button) :", queryctrl.name, queryctrl.id);
					break;
				case V4L2_CTRL_TYPE_BITMASK:
					//printf("%25s %#8.8x (bitmask): max=0x%08x default=0x%08x",
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;
					break;
				case V4L2_CTRL_TYPE_U8:
					//printf("%25s %#8.8x (u8)     : min=%d max=%d step=%d default=%d",
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;		
					break;
				case V4L2_CTRL_TYPE_U16:
					//printf("%25s %#8.8x (u16)    : min=%d max=%d step=%d default=%d",
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;		
					break;
				case V4L2_CTRL_TYPE_U32:
					//printf("%25s %#8.8x (u32)    : min=%d max=%d step=%d default=%d",
					(pcontrol + idx - 1)->uqueryctrl.minimum = queryctrl.minimum;
					(pcontrol + idx - 1)->uqueryctrl.maximum = queryctrl.maximum;
					(pcontrol + idx - 1)->uqueryctrl.step = queryctrl.step;
					(pcontrol + idx - 1)->uqueryctrl.default_value = queryctrl.default_value;		
					break;
				default:
					//printf("%25s %#8.8x (unknown): type=%x",
							//queryctrl.name, queryctrl.id, queryctrl.type);
					break;
			
        	}
        	CLEAR(control);
			control.id = queryctrl.id;
			(pcontrol + idx - 1)->ucontrol.id = control.id;
			
			if (0 == ioctl(fd, VIDIOC_G_CTRL, &control)) 
			{
				(pcontrol + idx - 1)->ucontrol.value = control.value;
				//printf(" value=%d %d ", control.value, (pcontrol + idx - 1)->ucontrol.value = control.value);
			}
			idx++;
			
        }
        
        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

/**
Function Name : releaseControls
Function Description : Frees the memory in control structure
Parameter : void
Return : void
**/
void releaseControls(void)
{
	if(pcontrol != NULL)
	{
		free(pcontrol);
		pcontrol = NULL;
	}
}

/**
Function Name : displayControls
Function Description : Displays the controls
Parameter : void
Return : void
**/
void displayControls(void)
{
	int index;
	printf("\n*********************************************************************\n");
	printf("\tFeature Control");
	printf("\n*********************************************************************\n");
	for(index = 1; index <= pcontrol->ctrlcount; index++)
	{
		printf("%d) %s\n", index, (pcontrol + index - 1)->uqueryctrl.name);
	}
	printf("%d) Exit", index);
} 

/**
Function Name : setControlValue
Function Description : Sets value to specific control
Parameter : Specific control option and value
Return : void
**/
void setControlValue(int option, int value)
{
	struct v4l2_control control;
	control.id =  (pcontrol + option - 1)->ucontrol.id;
	control.value = value;
	if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control) && errno != ERANGE) 
	{
		if(errno == EIO)
			printf("\nEnter manual mode and then change the value\n");
		else
        	perror("VIDIOC_S_CTRL");
        return;
    }
    (pcontrol + option - 1)->ucontrol.value = control.value;
    printf("\nValue Changed\n");
}

/**
Function Name : controlOption
Function Description : Displays control option for specific control
Parameter : Specific control option
Return : void
**/
void controlOption(int option)
{
	int value, ctrloption;
	printf("\n*********************************************************************\n");
	switch((pcontrol + option - 1)->uqueryctrl.type)
	{
		case V4L2_CTRL_TYPE_INTEGER:
					printf("%25s %#8.8x (int)    : min=%d max=%d step=%d default=%d",
					(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
					(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
					(pcontrol + option - 1)->uqueryctrl.step, (pcontrol + option - 1)->uqueryctrl.default_value);
				break;
				
				case V4L2_CTRL_TYPE_INTEGER64:
					printf("%25s %#8.8x (int64)  : min=%d max=%d step=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.step, (pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_STRING:
					printf("%25s %#8.8x (str)    : min=%d max=%d step=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.step);
					break;
				case V4L2_CTRL_TYPE_BOOLEAN:
					printf("%25s %#8.8x (bool)   : default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id, 
							(pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_MENU:
					printf("%25s %#8.8x (menu)   : min=%d max=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.default_value);
					//enumerateMenu((pcontrol + option - 1)->uqueryctrl);
					break;
				case V4L2_CTRL_TYPE_INTEGER_MENU:
					printf("%25s %#8.8x (intmenu): min=%d max=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.default_value);
					//enumerateMenu(queryctrl);
					break;
				case V4L2_CTRL_TYPE_BUTTON:
					printf("%25s %#8.8x (button) :", (pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id);
					break;
				case V4L2_CTRL_TYPE_BITMASK:
					printf("%25s %#8.8x (bitmask): max=0x%08x default=0x%08x",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id, 
							(pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U8:
					printf("%25s %#8.8x (u8)     : min=%d max=%d step=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.step, (pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U16:
					printf("%25s %#8.8x (u16)    : min=%d max=%d step=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.step, (pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				case V4L2_CTRL_TYPE_U32:
					printf("%25s %#8.8x (u32)    : min=%d max=%d step=%d default=%d",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id,
							(pcontrol + option - 1)->uqueryctrl.minimum, (pcontrol + option - 1)->uqueryctrl.maximum,
							(pcontrol + option - 1)->uqueryctrl.step, (pcontrol + option - 1)->uqueryctrl.default_value);
					break;
				default:
					printf("%25s %#8.8x (unknown): type=%x",
							(pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->uqueryctrl.id, 
							(pcontrol + option - 1)->uqueryctrl.type);
					break;
	}
	printf("\n*********************************************************************\n");
	
	while(1)
	{
		printf("1) Change %s\n", (pcontrol + option - 1)->uqueryctrl.name);
		printf("2) Exit\n");
		printf("\nEnter the option : ");
		getint(&ctrloption);
		if(ctrloption == 2)
			break;
		else if(ctrloption < 1 || ctrloption > 2)
		{
			printf("\nEnter the valid option\n");
			continue;
		}
		else
		{
			printf("\nNote Current %s value = %d", (pcontrol + option - 1)->uqueryctrl.name, (pcontrol + option - 1)->ucontrol.value);
			while(1)
			{
				printf("\nEnter the value : ");
				getint(&value);
				if(value < (pcontrol + option - 1)->uqueryctrl.minimum || value > (pcontrol + option - 1)->uqueryctrl.maximum)
				{
					printf("\nEnter valid value within range ");
					continue;
				}
				else
				{
					setControlValue(option, value);
					break;
				}
			}
			break;
		}
	}
}

/**
Function Name : controlFeature
Function Description : Controls the whole control feature
Parameter : integer
Return : void pointer
**/
int controlFeature()
{
	int option;
	if(loadControls() == NULL)
		return -1;
	while(1)
	{
		displayControls();
		printf("\nEnter the option : ");
		getint(&option);
		if(option == (pcontrol->ctrlcount + 1))
			break;
		else if(option > (pcontrol->ctrlcount + 1) ||  option < 1)
		{
			printf("\nEnter valid option");
			continue;
		}
		else
			controlOption(option);
		
	}
	releaseControls();
	return 0;
}

void* loadFormats()
{
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	struct v4l2_frmivalenum frmival;
	
	fmt.index = 0;
	fmt.type = type;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) 
		fmt.index++;
	format.fmtcount = fmt.index;
	if((format.ufmtdesc = (struct v4l2_ufmtdesc*)malloc(format.fmtcount * sizeof(struct v4l2_ufmtdesc))) == NULL)
	{
		printf("\nMalloc fails");
		return NULL;
	}
	fmt.index = 0;
	fmt.type = type;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) 
	{
		//printf(" %u ",fmt.index);
		//format.(*(ufmtdesc + fmt.index)).index = fmt.index;
		//format.ufmtdesc[fmt.index].index = fmt.index;
		//printf(" %u ",format.ufmtdesc[fmt.index].index);
		//format.ufmtdesc[fmt.index].type = fmt.type;
		//format.ufmtdesc[fmt.index].flags = fmt.
		//format.ufmtdesc[fmt.index].
		//format.ufmtdesc[fmt.index].
		memcpy(&format.ufmtdesc[fmt.index], &fmt, sizeof(fmt));
		//printf(" %u ",format.ufmtdesc[fmt.index].index);
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
			frmsize.index++;
		format.ufmtdesc[fmt.index].frmcount = frmsize.index;
		if((format.ufmtdesc[fmt.index].ufrmsizeenum = (struct v4l2_ufrmsizeenum*)malloc(format.ufmtdesc[fmt.index].frmcount * sizeof(struct v4l2_ufrmsizeenum))) == NULL)
		{
			printf("\nMalloc fails");
			return NULL;
		}
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) 
		{
			memcpy(&format.ufmtdesc[fmt.index].ufrmsizeenum[frmsize.index], &frmsize, sizeof(frmsize));
			
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) 
			{
				frmival.index = 0;
				frmival.pixel_format = fmt.pixelformat;
				frmival.width = frmsize.discrete.width;
				frmival.height = frmsize.discrete.height;
				while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0)
					frmival.index++;
				format.ufmtdesc[fmt.index].ufrmsizeenum [frmsize.index].fpscount = frmival.index;
				if((format.ufmtdesc[fmt.index].ufrmsizeenum [frmsize.index].frmivalenum = (struct v4l2_frmivalenum*)malloc(frmival.index * sizeof(struct v4l2_ufrmsizeenum))) == NULL)
				{
					printf("\nMalloc fails");
					return NULL;
				}
				
				frmival.index = 0;
				while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) >= 0)
				{
					memcpy(&format.ufmtdesc[fmt.index].ufrmsizeenum [frmsize.index].frmivalenum[frmival.index], &frmival, sizeof(frmival));
					frmival.index++;
				}
			}
			frmsize.index++;
		}
		fmt.index++;
	}
}

void displayFormats(void)
{
	int index;
	//printf("\nformat.fmtcount = %u", format.fmtcount);
	//printf("\nformat = %u %u", format.ufmtdesc[0].type, format.ufmtdesc[0].frmcount);
	for(index = 0; index < format.fmtcount; index++)
		printf("\n%d) %s", index + 1, format.ufmtdesc[index].description);
	printf("\n%d) Exit from Format", index + 1);
}

void displayResolution(int option)
{
	int index;
	for(index = 0; index < format.ufmtdesc[option - 1].frmcount; index++)
	{
		if(format.ufmtdesc[option - 1].ufrmsizeenum[index].type == V4L2_FRMSIZE_TYPE_DISCRETE)
		{
			printf("\n%d) Width*Height %uX%u", index+1, format.ufmtdesc[option - 1].ufrmsizeenum[index].discrete.width, format.ufmtdesc[option - 1].ufrmsizeenum[index].discrete.height);
		}
	}
	printf("\n%d) Exit from Resolution", index + 1);
}

void displayFPS(int format_option, int frame_option)
{
	int index;
	for(index = 0; index < format.ufmtdesc[format_option].ufrmsizeenum[frame_option].fpscount; index++)
	{
		if(format.ufmtdesc[format_option].ufrmsizeenum [frame_option].frmivalenum[index].type == V4L2_FRMIVAL_TYPE_DISCRETE)	
		{
			printf("\n%d) ", index + 1);
			fract2sec(format.ufmtdesc[format_option].ufrmsizeenum [frame_option].frmivalenum[index].discrete);
			fract2fps(format.ufmtdesc[format_option].ufrmsizeenum [frame_option].frmivalenum[index].discrete);
		}
	}
	printf("\n%d) Exit from FPS", index + 1);
}

void releaseFormats(void)
{
	int format_index, frame_index, fps_index;
	if(format.ufmtdesc != NULL)
	{
		for(format_index = 0; format_index < format.fmtcount; format_index++)
		{
			if(format.ufmtdesc[format_index].ufrmsizeenum != NULL)
			{
				for(frame_index = 0; frame_index < format.ufmtdesc[format_index].frmcount; frame_index++)
				{
					if(format.ufmtdesc[format_index].ufrmsizeenum [frame_index].frmivalenum != NULL)
					{
						free(format.ufmtdesc[format_index].ufrmsizeenum [frame_index].frmivalenum);
						format.ufmtdesc[format_index].ufrmsizeenum [frame_index].frmivalenum = NULL;
					}
				}
				free(format.ufmtdesc[format_index].ufrmsizeenum);
				format.ufmtdesc[format_index].ufrmsizeenum = NULL;
			}
		}
		free(format.ufmtdesc);
		format.ufmtdesc = NULL;
	}
}

int selectFormat(void)
{
	int format_option, frame_option, fps_option, valid_format = -1, valid_frame = -1, valid_fps = -1;
	char buf[10], *ptemp;
	struct v4l2_format fmt;
	struct v4l2_fmtdesc fmtdesc;
	if(loadFormats() == NULL)
	{
		releaseFormats();
		return -1;
	}
	while(1)
	{
		displayFormats();
		printf("\nEnter the option : ");
		getint(&format_option);
		if(format_option == format.fmtcount + 1)
			break;
		else if(format_option < 1 || format_option > format.fmtcount + 1)
		{
			printf("\nEnter Valid option");
			continue;
		}
		else
		{	
			valid_format = format_option;
			while(1)
			{
				displayResolution(format_option);
				printf("\nEnter the option : ");
				getint(&frame_option);
				if(frame_option == format.ufmtdesc[format_option - 1].frmcount + 1)
					break;
				else if(frame_option < 1 || frame_option > format.ufmtdesc[format_option - 1].frmcount + 1)
				{
					printf("\nEnter Valid option");
					continue;
				}
				else
				{
					
					valid_frame = frame_option;
					while(1)
					{
						displayFPS(format_option - 1, frame_option - 1);
						printf("\nEnter the option : ");
						getint(&fps_option);
						if(fps_option == format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].fpscount + 1)
							break;
						else if(fps_option < 1 || fps_option > format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].fpscount + 1)
						{
							printf("\nEnter Valid option");
							continue;
						}
						break;
					}
				}
				break;
			}
		}
		break;
	}
	
	if(capture_menu == 1)
	{
		//Capturing
		if(valid_format != -1 && valid_frame != -1)
		{
			cap_width       = format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].discrete.width;
			cap_height      = format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].discrete.height;
			cap_pix_format  = format.ufmtdesc[format_option - 1].pixelformat;
		}
		else if(valid_format != -1)
		{
			cap_pix_format = format.ufmtdesc[format_option - 1].pixelformat;
		}
	
		ptemp = (char*)&cap_pix_format;
		sprintf(buf, "%c%c%c%c", *ptemp, *(ptemp + 1), *(ptemp + 2), *(ptemp + 3) );
		cap_pix_format_str = strdup( buf );
	}
	else
	{
		//Streaming
		if(valid_format != -1 && valid_frame != -1)
		{
			width       = format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].discrete.width;
			height      = format.ufmtdesc[format_option - 1].ufrmsizeenum[frame_option - 1].discrete.height;
			pix_format  = format.ufmtdesc[format_option - 1].pixelformat;
		}
		else if(valid_format != -1)
		{
			pix_format = format.ufmtdesc[format_option - 1].pixelformat;
		}
	
		ptemp = (char*)&pix_format;
		sprintf(buf, "%c%c%c%c", *ptemp, *(ptemp + 1), *(ptemp + 2), *(ptemp + 3) );
		pix_format_str = strdup( buf );
	}
	printf("valid");
	releaseFormats();
	return 0;
}
