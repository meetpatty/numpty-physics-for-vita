/*
 * This file is part of NumptyPhysics
 * Copyright (C) 2008 Tim Edmonds
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
/*
* PSP port by rock88: rock88a@gmail.com
* http://rock88dev.blogspot.com
*/

#ifndef __SDL_LITE_H__
#define __SDL_LITE_H__

#include <stdint.h>
#include <string.h>
#include "Common.h"

#define SDL_RELEASED	0
#define SDL_PRESSED	1

typedef int8_t		Sint8;
typedef uint8_t		Uint8;
typedef int16_t		Sint16;
typedef uint16_t	Uint16;
typedef int32_t		Sint32;
typedef uint32_t	Uint32;

#define SDL_BUTTON_LEFT		1
#define SDL_BUTTON_MIDDLE	2
#define SDL_BUTTON_RIGHT	3

typedef enum {
	SDLK_NULL = 0,
	SDLK_q,
	SDLK_SPACE,
	SDLK_F4,
	SDLK_ASTERISK,
	SDLK_TAB,
	SDLK_F11,
	SDLK_F9,
	SDLK_x,
	SDLK_F8,
	SDLK_F7,
	SDLK_ESCAPE,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_LEFT,
	SDLK_RIGHT,
	SDLK_1,
	SDLK_3
} SDLKey;

typedef enum {
	KMOD_NONE  = 0x0000,
	KMOD_LSHIFT= 0x0001,
	KMOD_RSHIFT= 0x0002,
	KMOD_LCTRL = 0x0040,
	KMOD_RCTRL = 0x0080,
	KMOD_LALT  = 0x0100,
	KMOD_RALT  = 0x0200,
	KMOD_LMETA = 0x0400,
	KMOD_RMETA = 0x0800,
	KMOD_NUM   = 0x1000,
	KMOD_CAPS  = 0x2000,
	KMOD_MODE  = 0x4000,
	KMOD_RESERVED = 0x8000
} SDLMod;

typedef struct SDL_keysym {
	Uint8 scancode;			/* hardware specific scancode */
	SDLKey sym;			/* SDL virtual keysym */
	SDLMod mod;			/* current key modifiers */
	Uint16 unicode;			/* translated character */
} SDL_keysym;

typedef enum {
	SDL_NOEVENT = 0,			/* Unused (do not remove) */
	SDL_ACTIVEEVENT,			/* Application loses/gains visibility */
	SDL_KEYDOWN,			/* Keys pressed */
	SDL_KEYUP,			/* Keys released */
	SDL_MOUSEMOTION,			/* Mouse moved */
	SDL_MOUSEBUTTONDOWN,		/* Mouse button pressed */
	SDL_MOUSEBUTTONUP,		/* Mouse button released */
	SDL_QUIT,			/* User-requested quit */
} SDL_EventType;

typedef struct SDL_KeyboardEvent {
	Uint8 type;	/* SDL_KEYDOWN or SDL_KEYUP */
	Uint8 which;	/* The keyboard device index */
	Uint8 state;	/* SDL_PRESSED or SDL_RELEASED */
	SDL_keysym keysym;
} SDL_KeyboardEvent;

typedef struct SDL_MouseMotionEvent {
	Uint8 type;	/* SDL_MOUSEMOTION */
	Uint8 which;	/* The mouse device index */
	Uint8 state;	/* The current button state */
	Uint16 x, y;	/* The X/Y coordinates of the mouse */
	Sint16 xrel;	/* The relative motion in the X direction */
	Sint16 yrel;	/* The relative motion in the Y direction */
} SDL_MouseMotionEvent;

/* Mouse button event structure */
typedef struct SDL_MouseButtonEvent {
	Uint8 type;	/* SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP */
	Uint8 which;	/* The mouse device index */
	Uint8 button;	/* The mouse button index */
	Uint8 state;	/* SDL_PRESSED or SDL_RELEASED */
	Uint16 x, y;	/* The X/Y coordinates of the mouse at press time */
} SDL_MouseButtonEvent;

typedef struct SDL_QuitEvent {
	Uint8 type;	/* SDL_QUIT */
} SDL_QuitEvent;

typedef union SDL_Event {
	Uint8 type;
	SDL_KeyboardEvent key;
	SDL_MouseMotionEvent motion;
	SDL_MouseButtonEvent button;
	SDL_QuitEvent quit;
} SDL_Event;

typedef struct SDL_Rect {
	Sint16 x, y;
	Uint16 w, h;
} SDL_Rect;

extern int SDL_PollEvent(SDL_Event *event);
extern int SDL_PushEvent(SDL_Event *event);
extern void SDL_StartTicks(void);
extern Uint32 SDL_GetTicks();
extern int SDL_FillRect(char *dst, SDL_Rect *dstrect, Uint16 color);
extern int SDL_BlitSurface(char *dst, SDL_Rect *dstrect, char *src, SDL_Rect *srcrect);

extern unsigned short Color8888To5650(unsigned int Color32b);

#endif
