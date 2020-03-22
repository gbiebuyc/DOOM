// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <X11/extensions/XShm.h>
// Had to dig up XShm.c for this one.
// It is in the libXext, but not in the XFree86 headers.
#ifdef LINUX
int XShmGetEventBase( Display* dpy ); // problems with g++?
#endif

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1

Display*	X_display=0;
Window		X_mainWindow;
Colormap	X_cmap;
Visual*		X_visual;
GC		X_gc;
XEvent		X_event;
int		X_screen;
XVisualInfo	X_visualinfo;
XImage*		image;
int		X_width;
int		X_height;

// MIT SHared Memory extension.
boolean		doShm;

XShmSegmentInfo	X_shminfo;
int		X_shmeventtype;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean		grabMouse;
int		doPointerWarp = POINTER_WARP_COUNTDOWN;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;


#include <SDL2/SDL.h>
#include <time.h>

SDL_Window *window;
uint64_t start_time;
uint8_t current_palette[256 * 3];
SDL_Surface *screen_surface;

void I_ShutdownGraphics(void)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void I_StartFrame (void)
{
}

void I_GetEvent(void)
{
	SDL_Event e;

	printf("I_GetEvent called\n");
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN)
			I_Quit();
	}
}

void I_StartTic (void)
{
}


void I_UpdateNoBlit (void)
{
}

void I_FinishUpdate (void)
{
	if (time(0) - start_time > 3) I_Quit();
	int num_pixels = SCREENWIDTH * SCREENHEIGHT;
	for (int i = 0; i < num_pixels; i++) {
		uint32_t px;
		memcpy(&px, current_palette + screens[0][i] * 3, 3);
		px = (px >> 16 & 0xff) |
			(px & 0xff00) |
			(px & 0xff) << 16;
		((uint32_t*)screen_surface->pixels)[i] = px;
	}
	SDL_UpdateWindowSurface(window);

}


void I_ReadScreen (byte* scr)
{
}


void I_SetPalette (byte* palette)
{
	memcpy(current_palette, palette, sizeof(current_palette));
}


void I_InitGraphics(void)
{
	start_time = time(0);
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow( "DOOM",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREENWIDTH, SCREENHEIGHT,
	0
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return;
    }
    screen_surface = SDL_GetWindowSurface(window);
    screens[0] = malloc(SCREENWIDTH * SCREENHEIGHT);

}
