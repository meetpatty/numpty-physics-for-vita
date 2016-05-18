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

#include <psp2/display.h>
#include <PSP2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <vita2d.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "Overlay.h"
#include "Game.h"

int _isatty = 0;

#define BUF_WIDTH (960)
#define SCR_WIDTH (960)
#define SCR_HEIGHT (655)

static unsigned int __attribute__((aligned(16))) list[262144];

int ret;

int main(int argc, char* argv[])
{

#if 0	
	FILE *f = fopen("log.txt","w");
	fwrite("LOG\n",1,4,f);
	fclose(f);
#endif	
	vita2d_init();

	vita2d_start_drawing();
	vita2d_clear_screen();

	vita2d_end_drawing();
	vita2d_swap_buffers();

	/*void* fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);*/
	
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
	
	Game* game = new Game(0);

	while (true)//(running())
	{
		/*sceGuStart(GU_DIRECT,list);

		sceGuClearColor(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);*/

		vita2d_start_drawing();
		vita2d_clear_screen();
		

		game->run();

		vita2d_end_drawing();
		vita2d_swap_buffers();
		
		/*sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();*/
	}
	
	sceKernelExitProcess(0);
	return 0;
}
