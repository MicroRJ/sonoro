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



#define _log lgi_logFunction

#pragma warning(disable:4100)
#pragma warning(disable:4101)
#pragma warning(disable:4189)
#pragma warning(disable:4244)
#pragma warning(disable:4702)
#pragma warning(disable:4716)

/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */

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
#include "widget.c"
#include "engine.c"
#include "audio.c"

int pgraph_(lui_Box b, float *samples, int numsamples) {
	lui__drawBoxOutine(b,lgi_BLACK,4.f);
	int resolution = 1024;
	float window = b.x1 - b.x0;
	float pixelScale = window / resolution;
	float frequency = App.Audio.TestSignal.frequency;
	float volume = App.Audio.TestSignal.volume;
	/* By Using SAMPLE_RATE, We're Essentially Using Some Imaginary
 	Window SAMPLE_RATE Long (One Second Long), But We're Only Rendering The Visible Portion */
	double timeStep = 1. / (SAMPLE_RATE / frequency);
	double localTime = 0;
	lgi_Global double globalTime = 0.;
	for (int i=0; i<resolution; i+=1) {
		float sample = (float)ma_sind(MA_TAU_D * (globalTime + localTime)) * volume;
		float y = lui_remix(sample,-2.f,+2.f,b.y0,b.y1);
		localTime += timeStep;
		float x = b.x0 + i*pixelScale;
		lgi_drawCircleSDF((vec2){x,y},(vec2){1,1},lgi_BLACK,1.f,1.f);
		// lui__drawBox(lui_bbox(x,y,6,6),lgi_BLACK);
	}
	globalTime += lgi.Time.deltaSeconds;
}

void main(int c, char **v)  {
	lgi_initWindowed(1024,512,"Piano!");

	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\CascadiaCode.ttf",24);
	lui.font = fn;
	lui.textColor = lgi_BLACK;

	audiobegin();



	do {
		lgi_clearBackground(lgi_RGBA_U(0xfa,0xf9,0xf6,0xff));


		for (int y=1; y<32; y+=1) {
			for (int x=1; x<32; x+=1) {

				lgi_Color color = lgi_RGBA(.9f,.9f,.9f,1.f);
				lgi_drawLine(color,1,0,y*64.f,lgi.Window.size_x,y*64.f);
				lgi_drawLine(color,1,x*64.f-.5f,0,x*64.f-.5f,lgi.Window.size_y);
			}
		}

		lui_inibox();
		lui_setbox(rect_padd(lui_bbox(0,0,(float)lgi.Window.size_x,(float)lgi.Window.size_y),32.f,32.f));

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