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
//  DOOM graphics stuff for SDL2
//
//-----------------------------------------------------------------------------

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "doomdef.h"

#include <SDL2/SDL.h>

SDL_Window *window;
uint32_t current_palette[256];
SDL_Surface *screen_surface;

void I_ShutdownGraphics(void)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void I_StartFrame (void)
{
}

int sdl_to_doom_keycode(SDL_Keycode key) {
    if (key == SDLK_RIGHT)  return KEY_RIGHTARROW;
    if (key == SDLK_LEFT)   return KEY_LEFTARROW;
    if (key == SDLK_UP)     return KEY_UPARROW;
    if (key == SDLK_DOWN)   return KEY_DOWNARROW;
    if (key == SDLK_ESCAPE) return KEY_ESCAPE;
    if (key == SDLK_RETURN) return KEY_ENTER;
    if (key == SDLK_TAB)    return KEY_TAB;
    if (key == SDLK_F1)     return KEY_F1;
    if (key == SDLK_F2)     return KEY_F2;
    if (key == SDLK_F3)     return KEY_F3;
    if (key == SDLK_F4)     return KEY_F4;
    if (key == SDLK_F5)     return KEY_F5;
    if (key == SDLK_F6)     return KEY_F6;
    if (key == SDLK_F7)     return KEY_F7;
    if (key == SDLK_F8)     return KEY_F8;
    if (key == SDLK_F9)     return KEY_F9;
    if (key == SDLK_F10)    return KEY_F10;
    if (key == SDLK_F11)    return KEY_F11;
    if (key == SDLK_F12)    return KEY_F12;
    if (key == SDLK_BACKSPACE) return KEY_BACKSPACE;
    if (key == SDLK_PAUSE)  return KEY_PAUSE;
    if (key == SDLK_EQUALS) return KEY_EQUALS;
    if (key == SDLK_MINUS)  return KEY_MINUS;
    if (key == SDLK_RSHIFT) return KEY_RSHIFT;
    if (key == SDLK_LSHIFT) return KEY_RSHIFT;
    if (key == SDLK_RCTRL)  return KEY_RCTRL;
    if (key == SDLK_LCTRL)  return KEY_RCTRL;
    if (key == SDLK_RALT)   return KEY_RALT;
    if (key == SDLK_LALT)   return KEY_LALT;
    if (key >= ' ' && key <= '~') return key;
    return 0;
}

void I_StartTic (void)
{
    SDL_Event sdl_ev;
    event_t d_ev;

    while (SDL_PollEvent(&sdl_ev)) {
        if (sdl_ev.type == SDL_KEYDOWN) {
            d_ev.type = ev_keydown;
            if ((d_ev.data1 = sdl_to_doom_keycode(sdl_ev.key.keysym.sym)))
                D_PostEvent(&d_ev);
        }
        if (sdl_ev.type == SDL_KEYUP) {
            d_ev.type = ev_keyup;
            d_ev.data1 = sdl_to_doom_keycode(sdl_ev.key.keysym.sym);
            if ((d_ev.data1 = sdl_to_doom_keycode(sdl_ev.key.keysym.sym)))
                D_PostEvent(&d_ev);
        }
    }
}


void I_UpdateNoBlit (void)
{
}

void I_FinishUpdate (void)
{
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) {
        ((uint32_t*)screen_surface->pixels)[i] = current_palette[screens[0][i]];
    }
    SDL_UpdateWindowSurface(window);

}


void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


void I_SetPalette (byte* palette)
{
    for (int i = 0; i < 256; i++) {
        current_palette[i] =
            *palette++ << 16 |
            *palette++ << 8 |
            *palette++;
    }
}


void I_InitGraphics(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow( "DOOM",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREENWIDTH, SCREENHEIGHT,
            0);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return;
    }
    screen_surface = SDL_GetWindowSurface(window);
    screens[0] = malloc(SCREENWIDTH * SCREENHEIGHT);

}
