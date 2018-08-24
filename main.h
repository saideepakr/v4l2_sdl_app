/* MACROS */
#define VERSION "1.0"
#define DEVICE_PATH "/dev/video0"
#define OUTPUT_PATH "Output/default_file"
#define PIXEL_FORMAT "YUYV"
#define WIDTH 640
#define HEIGHT 480
#define BUF_SIZE 15
#define STILL_CAPTURE 1
#define STREAMING 2
#define FEATURE 3
#define EXIT 4

/* Global Varibles */
int fd = -1;
char *dev_path = "/dev/video0", *outfile = OUTPUT_PATH, *pix_format_str = PIXEL_FORMAT;
enum io_method io = IO_METHOD_MMAP;
struct buffer *buffers;
unsigned int n_buffers;
unsigned int width = WIDTH, height = HEIGHT, capture = 0, frame_count = 1, type = V4L2_CAP_VIDEO_CAPTURE, pix_format = v4l2_fourcc('Y', 'U', 'Y', 'V'), streaming = 1;
struct timeval start_time, end_time;
double elapsed_time;
pthread_t thread_control;
pthread_t thread_streaming;
CONTROLS *pcontrol = NULL;


/* Extern Varible */
extern int thread_exit_sig;

/* Function declaration */
void usage( FILE *fp, char * name );
int pixStr2pixU32(char* pix_format_str);
void getint(int* pnum);
void v4l2Menu(void);

/* Extern function declaration */
extern void mainstreamloop(void);
extern int controlFeature();
extern void captureMenu(void);
extern void streamingMenu(void);
extern void captureFun(void);
extern void *streamFun();
