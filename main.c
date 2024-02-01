#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#define ELANG_IMPLEMENTATION
#include <elang.h>

#define lgi_CLOSE_ON_ESCAPE
#define lgi_IMPLEMENTATION
#include <lgi.h>

#define lui_IMPLEMENTATION
#include <lui.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define _log lgi_logInfo

// char *nstr(char const *s) {
// 	int l = (int) strlen(s);
// 	char *r = malloc(l+1);
// 	strncpy(r,s,l+1);
// 	return r;
// }

#pragma warning(disable:4100)
#pragma warning(disable:4101)
#pragma warning(disable:4189)
#pragma warning(disable:4201)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4702)
#pragma warning(disable:4716)




struct {
	struct {
		ma_context context;
		ma_result lastError;
		struct {
			ma_device handle;
			struct {
				ma_device_info * array;
				ma_uint32        count;
			} Playback;
			struct {
				ma_device_info * array;
				ma_uint32        count;
			} Capture;
		} Device;

		struct {
			float frequency;
			// TODO: Can We Track This In Frames?
			double time;
			float volume;
		} TestSignal;
	} Audio;
} lgi_Global App;

#define FORMAT              ma_format_f32
#define CHANNELS            2
#define SAMPLE_RATE         48000

FILE *samplesfile;
#include <m_mix.c>
#include <m_box.h>
#include <view.h>
#include <theme.h>
#include <d_data.c>
#include <d_ini.c>

#include <n_node.h>
#include <n_object.c>
#include <n_node.c>
#include <n_base.c>

#include <n_math.c>

#include <n_time.c>
#include <n_tick.c>

#include <n_toggle.c>
#include <n_slider.c>
#include <n_detector.c>

#include <n_graph.c>

#include <n_osc.c>
#include <n_dac.c>
#include <n_engine.c>

#include <n_global.c>

#include "audio.c"


void main(int c, char **v)  {
	lgi_initWindowed(720*2,720,"Sonoro");

	samplesfile = fopen(".smp","wb");

	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\static\\CascadiaCode-SemiBold.ttf",18);
	lui.font = fn;
	lui.textColor = lgi_BLACK;



	loadbuiltins();


	load();

	audiobegin();

	float xclick = 0, yclick = 0;
	float xoffset_ = 0, yoffset_ = 0;
	do {
		if (lgi_isButtonDown(2)) {
			if (!lgi_wasButtonDown(2)) {
				xclick = lgi.Input.Mice.xcursor;
				yclick = lgi.Input.Mice.ycursor;
				xoffset_ = xoffset;
				yoffset_ = yoffset;

            // {
            //     HMENU hContextMenu = CreatePopupMenu();
            //     AppendMenu(hContextMenu, MF_STRING, 1, "Option 1");
            //     AppendMenu(hContextMenu, MF_STRING, 2, "Option 2");

            //     POINT cursor;
            //     GetCursorPos(&cursor);
            //     TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, cursor.x, cursor.y, 0, lgi.Window.win32.handle, NULL);
            //     // DestroyMenu(hContextMenu);
            // }
			}
			xoffset = xoffset_ - (xclick - lgi.Input.Mice.xcursor);
			yoffset = yoffset_ - (yclick - lgi.Input.Mice.ycursor);
		} else {
			if (lgi_wasButtonDown(2)) {
			}
		}

		for (int i=0; i<arrlen(module); i+=1) {
			if (lgi_testKey('1'+i)) {
				addnode(newobj(module[i]));
			}
		}

		t_class *tickclass = findclass("tick");
		for (int i=0; i<arrlen(drawlist); i+=1) {
			t_node *n = drawlist[i];
			if (n->pclass == tickclass) {
				if (lgi_testKey(' ')) {
					int results = execnode(n,0,0);
					while (results --) pop();
				}
			}
		}
		// Sleep(1000);


		lgi_clearBackground(UI_COLOR_BACKGROUND);

		drawgrid();
		editor();

		for (int i=0; i<arrlen(drawlist); i+=1) {
			drawnode(drawlist[i]);
		}
	} while (lgi_tick());

	audioend();

	save();
	fclose(samplesfile);
}

#if 0
int main2 (int c, char **v) {
	(void) c;
	(void) v;

	// main2();s

	ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		return result;
	}

	// char stuff[MAX_PATH];
	// GetCurrentDirectory(sizeof(stuff),stuff);

	// lui_Font *fn = lui_loadFont("lui\\assets\\Gloria_Hallelujah\\GloriaHallelujah-Regular.ttf",64);
	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\CascadiaCode.ttf",14);
	lui.font = fn;


	lgi_initWindowed(1024,512,"Piano!");

	unsigned char keys[0xff] = {0};
	do {
		// play the sounds
		for (int i=1; i<10; i+=1) {
			if (lgi_testKey('0'+i)) {
				ma_engine_play_sound(&engine,_fmt("samples/piano/a%i.mp3",49+i-1),NULL);
				keys[i-1] = 255;
			} else {
				if (keys[i-1] > 0) {
					keys[i-1] -= 1;
				}
			}
		}

		// drawing
		lgi_clearBackground(lgi_RED);

		lgi_Global float slider = 0;
		// slider = (float) (.5f+.5f*sin(lgi_TAU * lgi.Time.total_seconds));
		wdg_slider(lui_bbox(24.f,lgi.Window.size_y-128.f,256,32),0,1.,&slider);


		float yboard = 24.f;
		for (int i=0; i<12; i+=1) {
			lgi_Color color = lgi_WHITE;
			if (keys[i] != 0) {
				float t = keys[i]/255.f;
				color = vec4_mix(powf(t,12.2f),lgi_WHITE,lgi_OLIVE);
				lui_setTextColor(vec4_mix(powf(t,2.2f),lgi_CLEAR,lgi_WHITE));
				lui__drawText(lui_dobox(0,lgi.Window.size_y - 128.f,256,256),_fmt("key - %i",i));
			}
			lgi_drawBox((vec2){i*25.f + i*2.f,yboard},(vec2){25.f,100.f},color,5,1);
		}

		for (int i=0; i<3; i+=1) {
			lgi_Color color = lgi_BLACK;
			color.r *= .75f;
			color.g *= .75f;
			color.b *= .75f;
			lgi_drawBox((vec2){25.f-6.f + i*25.f + i*2.f,yboard + 50.f},(vec2){12.f,50.f},color,1,1);
		}


	} while (lgi_tick());

	return 0;
}
#endif