/* ya2d sample by xerpi */

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
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int run = 1;
int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);

int main(int argc, char* argv[])
{
    SetupCallbacks();
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    ya2d_init();
    
    struct ya2d_texture *t;
    t = ya2d_load_JPEG_buffer(test3_jpg_start, test3_jpg_size, YA2D_PLACE_VRAM);
    if (t == NULL) goto exit;
    
    ya2d_swizzle_texture(t);
    
    float angle = 0.0f;
    float cross_x = 50, cross_y = 50;
    SceCtrlData pad, old_pad; old_pad.Buttons = 0;
    int centered = 0, rotate = 0, vsync = 0;
    
    while(run) {
        sceCtrlPeekBufferPositive(&pad, 1);
        ya2d_start_drawing();
        ya2d_clear_screen(0);
        
        if (pad.Buttons & PSP_CTRL_RTRIGGER) angle += 0.005f;
        else if (pad.Buttons & PSP_CTRL_LTRIGGER) angle -= 0.005f;
        
        if (pad.Buttons & PSP_CTRL_CROSS & ~old_pad.Buttons) centered = !centered;
        if (pad.Buttons & PSP_CTRL_TRIANGLE & ~old_pad.Buttons) rotate = !rotate;
        if (pad.Buttons & PSP_CTRL_SQUARE & ~old_pad.Buttons) ya2d_set_vsync(vsync = !vsync);
    
        if (fabs(pad.Lx-128) > 60) cross_x += (pad.Lx-128)/100.0f;
        if (fabs(pad.Ly-128) > 60) cross_y += (pad.Ly-128)/100.0f;
        
        if (rotate)
            ya2d_draw_texture_rotate(t, cross_x, cross_y, angle);
        else {
            if (centered)
                ya2d_draw_texture_centered(t, cross_x, cross_y);
            else
                ya2d_draw_texture(t, cross_x, cross_y);
        }

        ya2d_draw_rect(cross_x, cross_y, t->width, t->height, 0xFF00FF00, 0);
        
        ya2d_draw_line(cross_x-5, cross_y, cross_x+5, cross_y, 0xFF0000FF);
        ya2d_draw_line(cross_x, cross_y-5, cross_x, cross_y+5, 0xFF0000FF);

        tinyfont_draw_stringf(265, 10,  GU_RGBA(0,0,255,255), "FPS: %.2f  angle: %.2f", ya2d_get_fps(), angle);
        tinyfont_draw_stringf(265, 20,  GU_RGBA(0,0,255,255), "x: %.2f   y: %.2f", cross_x, cross_y);
        if (centered) tinyfont_draw_string(265, 30,  GU_RGBA(255,0,255,255), "centered");
        if (rotate)   tinyfont_draw_string(265, 40,  GU_RGBA(0,255,255,255), "rotate");
        if (vsync)    tinyfont_draw_string(265, 50,  GU_RGBA(255,0,0,255), "vsync");
        tinyfont_draw_string16x16(10, 60,  GU_RGBA(35,153,45,255), "16x16 Draw test!");
        
        
        tinyfont_draw_rotated_string(120, 150, GU_RGBA(255,255,0,255), angle, "Rotated text!!");
        
        ya2d_finish_drawing();
        ya2d_swapbuffers();
        ya2d_calc_fps();
        old_pad = pad;
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
