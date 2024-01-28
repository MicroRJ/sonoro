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
#pragma warning(disable:4702)
#pragma warning(disable:4716)

lgi_API void drawboxoutline(lui_Box b, float thickness, lgi_Color color) {
	lgi_drawQuad(color,b.x0-thickness,b.y1          ,b.x1-b.x0+thickness*2,thickness);
	lgi_drawQuad(color,b.x0-thickness,b.y0-thickness,b.x1-b.x0+thickness*2,thickness);
	lgi_drawQuad(color,b.x0-thickness,b.y0,thickness,b.y1-b.y0);
	lgi_drawQuad(color,b.x1          ,b.y0,thickness,b.y1-b.y0);
}



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

#include "lui_mix.c"
#include "audio.c"
#include <theme.h>
#include <n_node.c>
#include <n_math.c>
#include <n_slider.c>
#include <n_graph.c>
#include <n_osc.c>
#include "engine.c"

void main(int c, char **v)  {
	lgi_initWindowed(1024,512,"Piano!");

	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\CascadiaCode.ttf",18);
	lui.font = fn;
	lui.textColor = lgi_BLACK;

	audiobegin();

	addnode(numnode(0));
	addnode(numnode(1));
	addnode(minnode());
	addnode(minnode());
	addnode(graphnode("graph",1024));
	addnode(oscnode(440));

	t_node *frq = slidernode("Hz",1,440,110);
	t_node *dbv = slidernode("Db",0,1,.2f);
	addnode(frq);
	addnode(dbv);

	addedge(drawlist[0],0,drawlist[2],0);
	addedge(drawlist[1],0,drawlist[2],1);
	addedge(drawlist[2],0,drawlist[3],0);
	addedge(drawlist[1],0,drawlist[3],1);



	do {
		lgi_clearBackground(lgi_RGBA_U(0xfa,0xf9,0xf6,0xff));

		lgi_Color color = lgi_RGBA(.956f,.956f,.956f,1.f);
		for (int y=1; y<32; y+=1) {
			lgi_drawLine(color,1,0,y*32.f,lgi.Window.size_x,y*32.f);
		}
		for (int x=1; x<32; x+=1) {
			lgi_drawLine(color,1,x*32.f-.5f,0,x*32.f-.5f,lgi.Window.size_y);
		}

		if (lgi_testKey(' ')) {
			exec();

		}
		App.Audio.TestSignal.frequency = ((t_slider *)frq)->val;


		lgi_clearBackground(UI_COLOR_BACKGROUND);

#if 1
		if (selinletnode != lgi_Null) {
			float xcursor = (float) lgi.Input.Mice.xcursor;
			float ycursor = (float) lgi.Input.Mice.ycursor;
			t_box in = getinletbox(nodebox(selinletnode),selinletslot);
			lgi_drawLine(lgi_RED,2.f,in.x0,in.y0,xcursor,ycursor);

			for (int i=0; i<arrlen(drawlist); i+=1) {
				t_node *t = drawlist[i];
				if (t == selinletnode) continue;
				lui_Box b = t->box;

				for (int j=0; j<t->numoutlets; j+=1) {
					lui_Box h = lui_bbox(b.x0+4+j*16+j*8,b.y0,16,8);
					// lui__drawBox(h,lgi_GREEN);
					if (lui_testinbox(h,xcursor,ycursor)) {

						/* We're drawing an inlet to an outlet */
						addedge(t,j,selinletnode,selinletslot);

						lgi_logInfo("attached!");
						selinletnode = lgi_Null;
						selinletslot = 0;
						break;
					}
				}
			}
		}
#endif
		for (int i=0; i<arrlen(drawlist); i+=1) {
			drawnode(drawlist[i]);
		}

#if 0
		lui_inibox();
		lui_setbox(lui_Box_enlarge(lui_bbox(0,0,(float)lgi.Window.size_x,(float)lgi.Window.size_y),32.f,32.f));

		lui_cutbox(lui_top,32.f);
		wdg_slider(lui_cutbox(lui_left,256.f),	1.f,440.f,&App.Audio.TestSignal.frequency);
		lui_popbox();
		lui_popbox();

		lui_cutbox(lui_top,32.f);
		wdg_slider(lui_cutbox(lui_left,256.f),	0.f,1.f,&App.Audio.TestSignal.volume);
		lui_popbox();
		lui_popbox();

		lui_cutbox(lui_top,32.f);
		lui_popbox();

		pgraph_(lui_cutbox(lui_top,256.f),NULL,0);
#endif

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