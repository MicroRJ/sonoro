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
#define _err lgi_logError
#define _war lgi_logError

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
#include <n_flow.c>

#include <n_toggle.c>
#include <n_slider.c>

#include <n_graph.c>

#include <n_osc.c>
#include <n_dac.c>
#include <n_engine.c>

#include <n_global.c>

#include "audio.c"


char *keyToClass[][2] = {
	{"N","num"},
	{"A","dac"},
	{"G","graph"},
	{"T","timer"},
	{"D","detector"},
	{"O","osc"},
	{"K","tick"},
	{"0","add"},
	{"1","div"},
	{"2","min"},
	{"F","toggle"},
	{"S","slider"},
	{"Y","splitter"},
};

void main(int c, char **v)  {
	lgi_initWindowed(720*2,720,"Sonoro");

	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\static\\CascadiaCode-SemiBold.ttf",18);
	lui.font = fn;
	lui.textColor = lgi_BLACK;

	App.Editor.wireType = 1;
	loadbuiltins();

	for (int i=0; i<_countof(keyToClass); i+=1) {
		_log("key mapping: %c => %s",keyToClass[i][0][0],keyToClass[i][1]);
	}

	int loadedconfig = lgi_False;

	for (int i=1; i<c; i+=1) {
		_log("loading config");
		loadedconfig = loadconfig(v[i]);
		if (!loadconfig) {
			_err("'%s': failed to load config file",v[i]);
		}
		break;
	}

	if (!loadedconfig) {
		loadedconfig = loadconfig(0);
	}

	audiobegin();

	float xclick = 0, yclick = 0;
	float xoffset_ = 0, yoffset_ = 0;
	do {
		yoffset -= lgi.Input.Mice.yscroll * 4.;
		xoffset -= lgi.Input.Mice.xscroll * 4.;
		if (lgi_isButtonDown(2)) {
			if (!lgi_wasButtonDown(2)) {
				xclick = lgi.Input.Mice.xcursor;
				yclick = lgi.Input.Mice.ycursor;
				xoffset_ = xoffset;
				yoffset_ = yoffset;

			}
			xoffset = xoffset_ - (xclick - lgi.Input.Mice.xcursor);
			yoffset = yoffset_ - (yclick - lgi.Input.Mice.ycursor);
		} else {
			if (lgi_wasButtonDown(2)) {
			}
		}

		if (lgi_testKey('W')) {
			App.Editor.wireType += 1;
			if (App.Editor.wireType >= 4) {
				App.Editor.wireType = 1;
			}
		}

		if (!lgi_testCtrlKey()) {
			for (int i=0; i<_countof(keyToClass); i+=1) {
				if (lgi_testKey(keyToClass[i][0][0])) {
					_log("adding object %s",keyToClass[i][1]);
					addnode(newobj(findclass(keyToClass[i][1])));
					break;
				}
			}
		}

		if (lgi_testCtrlKey() && lgi_testKey('S')) {
			saveconfig();
			_log("config saved");
		}

		t_class *tickclass = findclass("tick");
		for (int i=0; i<arrlen(drawlist); i+=1) {
			t_node *n = drawlist[i];
			if (n->pclass == tickclass) {
				if (lgi_testKey(' ')) {
					test_execorder(n);
					// int results = execnode(n,0,0);
					// while (results --) pop();
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

		lui__drawBox(lui_bbox(lgi.Input.Mice.xcursor,lgi.Input.Mice.ycursor,4,4),wirecolor(App.Editor.wireType));


		//
		lui_Box menubox = lui_bbox(0,lgi.Window.size_y-32,lgi.Window.size_x, 32);
		lui__drawBox(menubox,lgi_BLACK);

		lgi_drawOutline(32,menubox.y0+8,256,16,2,lgi_WHITE);
		float p = ((float)istack)/_countof(stack) * (256-8);
		lgi_drawQuad(lgi_WHITE,32+4,menubox.y0+8+4,p,16-8);


		lui_Box list = lui_bbox(0,0,lgi.Window.size_x,lgi.Window.size_y-32);
		for (int i = 0; i < istack; i += 1) {
			int k = stack[i].k;
			char const *s = _fmt("%i: %s",i,typetostring(k));
			if (k == VSTRING) {
				s = _fmt("%s = %s",s,stack[i].s->contents);
			} else
			if (k == VFLOAT) {
				s = _fmt("%s = %f",s,stack[i].f);
			} else
			if (k == VINT) {
				s = _fmt("%s = %i",s,stack[i].i);
			} else {
				__debugbreak();
			}
			lui__drawText(lui_boxcut(&list,lui_top,32),s);
		}

	} while (lgi_tick());

	audioend();
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