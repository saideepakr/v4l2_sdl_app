#include "stream.h"

/**
Function Name : v4l2_streaming
Function Description : Create surface, renderer, texture and infinitly read the frame till exit
Parameter : void
Return : void
**/
void *v4l2_streaming() {
	// SDL2 begins
	CLEAR(sdlRect);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) 
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return NULL;
	}

	sdlScreen = SDL_CreateWindow("Simple YUV Window", SDL_WINDOWPOS_UNDEFINED,
		                       SDL_WINDOWPOS_UNDEFINED, width,
		                       height, SDL_WINDOW_SHOWN);

	if (!sdlScreen) 
	{
		fprintf(stderr, "SDL: could not create window - exiting:%s\n",
				SDL_GetError());
		return NULL;
	}
	
	
	/*SDL_RendererInfo* info;
	if(SDL_GetRenderDriverInfo(1, info) != 0)
    {
    	printf("ERROR %s",  SDL_GetError());
    	return NULL;
    }

	printf("%s %d %d %d %d\n", info->name, info->flags, info->num_texture_formats, info->max_texture_width , info->max_texture_height);*/
	
	//printf(" %d \n",SDL_GetNumRenderDrivers());
	sdlRenderer = SDL_CreateRenderer(
	  sdlScreen, -1, SDL_RENDERER_SOFTWARE/*SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC*/);
	
	if (sdlRenderer == NULL) 
	{
		fprintf(stderr, "SDL_CreateRenderer Error\n");
		return NULL;
	}
	
	sdlTexture =
	  SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_YUY2,
		                SDL_TEXTUREACCESS_STREAMING, width, height);
		                
    if(sdlTexture == NULL)
    {
    	fprintf(stderr, "SDL_CreateTexture Error\n");
    	return NULL;
    }
	sdlRect.w = width;
	sdlRect.h = height;

	TTF_Init();
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 16); //this opens a font style and sets a size
	if (font == NULL) 
	{
        fprintf(stderr, "error: font not found\n");
		return NULL;
	}

	SDL_Color Red = {255, 0, 0};  // this is the color in rgb format, maxing out all would give you the color Red, and it will be your text's color

	surfaceMessage = TTF_RenderText_Solid(font, "0.00ms  -   0.0fps", Red); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
	
	if(surfaceMessage == NULL)
		                {
		                	printf("Error is here");
		                	return NULL;
		                }

	Message = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage); //now you can convert it into a texture
	
	if(Message == NULL)
		                {
		                	printf("Error is here");
		                	return NULL;
		                }

	 //create a rect
	Message_rect.x = 0;  //controls the rect's x coordinate 
	Message_rect.y = 0; // controls the rect's y coordinte
	Message_rect.w = surfaceMessage->w; // controls the width of the rect
	Message_rect.h = surfaceMessage->h; // controls the height of the rect
	
	gettimeofday(&start_time, NULL);
	
	while (!thread_exit_sig) 
	{
		read_frame();
		
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);	

		gettimeofday(&end_time, NULL);

		elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0;      // sec to ms
		elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0;   // us to ms

		sprintf(fps_buf, "%.2lfms - %.1lffps", elapsed_time, 1000/elapsed_time);

		surfaceMessage = TTF_RenderText_Solid(font, fps_buf, Red); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
		
		if(surfaceMessage == NULL)
		                {
		                	printf("Error is here");
		                	return NULL;
		                }

		Message = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage); //now you can convert it into a texture

		if(Message == NULL)
		                {
		                	printf("Error is here");
		                	return NULL;
		                }


		start_time = end_time;
	}
	
	return NULL;
	
}

/**
Function Name : frame_handler
Function Description : Create surface, renderer, texture and infinitly read the frame till exit
Parameter : Start address and length of the frame
Return : void
**/
void frame_handler(void *pframe, int length) 
{
	SDL_UpdateTexture(sdlTexture, &sdlRect, pframe, stream_width * 2);
	//  SDL_UpdateYUVTexture
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
	SDL_RenderCopy(sdlRenderer, Message, NULL, &Message_rect);
	SDL_RenderPresent(sdlRenderer);
}

/**
Function Name : mainstreamloop
Function Description : Main loop to create pthread and stream the frame
Parameter : void
Return : void
**/
void mainstreamloop()
{
	if (pthread_create(&thread_stream, NULL, v4l2_streaming, NULL))
	{
		fprintf(stderr, "create thread failed\n");
		return;
  	}
	int quit = 0;
	SDL_Event e;
	stream_width = width;
	while (!quit) 
	{
		while (SDL_PollEvent(&e)) 
		{
			if (e.type == SDL_QUIT) { // click close icon then quit
				quit = 1;
			}
			if (e.type == SDL_KEYDOWN) 
			{
				if (e.key.keysym.sym == SDLK_ESCAPE) // press ESC the quit
					quit = 1;
			}
		}
		if(thread_exit_sig == 1)
			quit = 1;
		usleep(25);
	}
	thread_exit_sig = 1;               // exit thread_stream
	pthread_join(thread_stream, NULL); // wait for thread_stream exiting
	thread_exit_sig = 0; 
	SDL_DestroyTexture(Message);
	SDL_DestroyTexture(sdlTexture);
	SDL_FreeSurface(surfaceMessage);
 	TTF_CloseFont(font);
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlScreen);
	TTF_Quit();
	SDL_Quit();
}


void *streamFun()
{
	thread_stream_complete = 0;
	openDevice(dev_path);	
	init_device();	
	start_capturing();	
	mainstreamloop();
	stop_capturing();
	uninit_device();
	close_device();
	thread_stream_complete = 1;
	return NULL;
}

void display_streamingMenu(void)
{
	printf("\n*********************************************************************\n");
	printf("\t\tStreaming Menu");
	printf("\n*********************************************************************\n");
	printf("\n1) Select Format");
	printf("\n2) Stream ON");
	printf("\n3) Stream OFF");
	printf("\n4) Exit from Streaming menu");
}

void streamingMenu(void)
{
	int option;
	stream_menu = 1;
	while(1)
	{
		display_streamingMenu();
		printf("\nEnter the option : ");
		getint(&option);
		switch(option)
		{
			case SELECT_FORMAT:
				if(selectFormat() != 0)
					option = EXIT_STREAMING;
				break;
			case STREAM_ON:
				if(thread_stream_complete == 0)
					printf("\nAlready streaming");
				else
				{
					if (pthread_create(&thread_streaming, NULL, streamFun, NULL))
					{
						fprintf(stderr, "create thread failed\n");
						option = EXIT_STREAMING;
				  	}
				}
				break;
			case STREAM_OFF:
				thread_exit_sig = 1;
				pthread_join(thread_streaming, NULL);
				break;
			case EXIT_STREAMING:
				break;
			default: 
				printf("\nEnter valid option");
		}
		if(option == EXIT_STREAMING)
			break;
	}
	stream_menu = 0;
}
