#include "v4l2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define MAX_FPS_BUF 50
#define SELECT_FORMAT 1
#define STREAM_ON 2
#define STREAM_OFF 3
#define EXIT_STREAMING 4

/* Global declaration*/
pthread_t thread_stream;
SDL_Window *sdlScreen;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Rect sdlRect;
SDL_Texture* Message;
TTF_Font* font;
SDL_Surface* surfaceMessage;
SDL_Rect Message_rect;
char fps_buf[MAX_FPS_BUF];
unsigned int stream_width;

/* miscellanous */
int thread_exit_sig = 0;

/* Function declaration */
void frame_handler(void *pframe, int length);
void *v4l2_streaming();
void mainstreamloop(void);
void *streamFun();
void streamingMenu(void);
void display_streamingMenu(void);

/* Extern variable declaration */
extern int fd;
extern char *dev_path, *outfile, *pix_format_str;
extern enum io_method io;
extern struct buffer *buffers;
extern unsigned int n_buffers;
extern unsigned int width , height, capture, frame_count, type, pix_format;
extern struct timeval start_time, end_time;
extern double elapsed_time;
extern int thread_stream_complete, capture_menu, stream_menu;
extern pthread_t thread_streaming;

/* Extern function declaration */
extern int read_frame(void);
extern void getint(int* pnum);
extern void stop_capturing(void);
extern void start_capturing(void);
extern void uninit_device(void);
extern void init_device(void);
extern void openDevice(char* dev_path);
extern void close_device(void);
extern int selectFormat(void);
