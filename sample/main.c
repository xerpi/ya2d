/*
* Copyright (C) 2013, xerpi
*/

/* ya2d sample */

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <pspgu.h>
#include <pspgum.h>
#include <vram.h>
#include <tinyfont.h>
#include <ya2d.h>

extern unsigned char test3_jpg_start[];
extern unsigned int  test3_jpg_size;

PSP_MODULE_INFO("ya2d Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

int run = 1;
int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);

int main(int argc, char* argv[])
{
	pspDebugScreenInit();
	SetupCallbacks();
	
	ya2d_init();	
	ya2d_set_clear_color(0);
	
	ya2d_Texture *t;
	t = ya2d_load_JPEG_buffer(test3_jpg_start, test3_jpg_size, YA2D_PLACE_VRAM);
	if (t == NULL) goto exit;
	
	ya2d_swizzle_texture(t);
	
	float angle = 0.0f;
	SceCtrlData pad;
	
	while(run) {
		sceCtrlPeekBufferPositive(&pad, 1);
		ya2d_start();
		
		if (pad.Buttons & PSP_CTRL_RTRIGGER) angle += 0.005f;
		else if (pad.Buttons & PSP_CTRL_LTRIGGER) angle -= 0.005f;
		
		ya2d_draw_rotate_texture(130, 130, angle, t);
		
		
		tinyfont_draw_stringf(265, 10,  GU_RGBA(0,0,255,255), "FPS: %.2f  angle: %f", ya2d_get_fps(), angle);
		
		ya2d_finish();
		ya2d_swapbuffers();
	}
	
	ya2d_free_texture(t);
exit:
	ya2d_shutdown();
	sceKernelExitGame();
	return 0;
}


/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	run = 0;
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}
