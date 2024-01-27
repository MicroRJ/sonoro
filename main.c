

#include <stdio.h>

#define lgi_CLOSE_ON_ESCAPE
#define lgi_IMPLEMENTATION
#include <lgi.h>

#define lui_IMPLEMENTATION
#include <lui.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define _log lgi_logFunction

#pragma warning(disable:4100)
#pragma warning(disable:4702)
#pragma warning(disable:4716)

struct {
	struct {
		ma_context context;
		ma_result lastError;
		struct {
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


float lui_mix(float ratio, float min, float max) {
	return min + (max - min) * ratio;
}

float lui_unmix(float val, float min, float max) {
	return (val - min) / (max - min);
}

float lui_remix(float val, float val_min, float val_max, float min, float max) {
	return lui_mix(lui_unmix(val,val_min,val_max),min,max);
}

float lui_clip(float val, float min, float max) {
	return val < min ? min : val > max ? max : val;
}

int slider_(lui_Box b, float min, float max, float *value) {
	lgi_Global float xclick;

	float xcursor = (float) lgi.Input.Mice.xcursor;
	float ycursor = (float) lgi.Input.Mice.ycursor;
	if (lui_testinbox(b,xcursor,ycursor)) {
		if (lgi_isButtonDown(0)) {
			if (!lgi_wasButtonDown(0)) {
				xclick = xcursor;
			}
			float xdelta = xcursor - b.x0;
			*value = lui_remix(xdelta,0,b.x1-b.x0-8.f,min,max);
		}
	}

	*value = lui_clip(*value,min,max);
	lui_Box t = b;
	t.x0 = lui_remix(*value,min,max,b.x0,b.x1-8.f);
	t.x1 = t.x0 + 8.f;
	lui__drawBox(b,lgi_WHITE);
	lui__drawBox(t,lgi_BLACK);
	lui__drawBoxOutine(b,lgi_BLACK,4.f);
	lui__drawText(b,_fmt("%.2fHz",*value));

}

int pgraph_(lui_Box b, float *samples, int numsamples) {
	lui__drawBox(b,lgi_WHITE);
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

void writeTestSignal(float *output, int frameCount) {
	// _log(lgi_INFO,_fmt("Frame Count %i",frameCount));
	double frequency = App.Audio.TestSignal.frequency;
	double advance = 1. / (SAMPLE_RATE / frequency);

	double time = App.Audio.TestSignal.time;
	for (int iFrame = 0; iFrame < frameCount; iFrame += 1) {
		float sample = (float) (ma_sind(MA_TAU_D * time)) * App.Audio.TestSignal.volume;
		time += advance;
		for (int iChannel = 0; iChannel < CHANNELS; iChannel += 1) {
			output[iFrame*CHANNELS + iChannel] = sample;
		}
	}
	App.Audio.TestSignal.time = time;
}

void data_callback(ma_device* device, void *output, const void *input, ma_uint32 frameCount) {
	writeTestSignal(output,frameCount);
	// if (output != NULL && input != NULL) {
	// 	MA_COPY_MEMORY(output,input,frameCount*ma_get_bytes_per_frame(device->capture.format,device->capture.channels));
	// }
}

void main(int c, char **v)  {
	lgi_initWindowed(1024,512,"Piano!");

	lui_Font *fn = lui_loadFont("lui\\assets\\CascadiaCode\\CascadiaCode.ttf",24);
	lui.font = fn;
	lui.textColor = lgi_BLACK;

	App.Audio.TestSignal.frequency = 110;
	App.Audio.TestSignal.volume    = .2f;

	App.Audio.lastError = ma_context_init(NULL,0,NULL,&App.Audio.context);
	App.Audio.lastError = ma_context_get_devices(&App.Audio.context
	,	&App.Audio.Device.Playback.array, &App.Audio.Device.Playback.count
	,	&App.Audio.Device. Capture.array, &App.Audio.Device. Capture.count);

	ma_device_config config = ma_device_config_init(ma_device_type_duplex);

	config.playback.format    = FORMAT;
	config.playback.channels  = CHANNELS;
	config.playback.pDeviceID = NULL;

	config.capture. format    = FORMAT;
	config.capture. channels  = CHANNELS;
	config.capture. pDeviceID = NULL;

	config.sampleRate         = SAMPLE_RATE;
	config.dataCallback       = data_callback;
	config.pUserData          = NULL;

	ma_device device;
	if (ma_device_init(&App.Audio.context, &config, &device) != MA_SUCCESS) {
		return;
	}

	ma_device_start(&device);


	do {
		lgi_clearBackground(lgi_RGBA_U(0xfa,0xfa,0xfa,0xff));

		lui_inibox();
		lui_setbox(rect_padd(lui_bbox(0,0,(float)lgi.Window.size_x,(float)lgi.Window.size_y),32.f,32.f));

		lui_cutbox(lui_top,32.f);
		slider_(lui_cutbox(lui_left,256.f),	1.f,440.f,&App.Audio.TestSignal.frequency);
		lui_popbox();
		lui_popbox();

		lui_cutbox(lui_top,32.f);
		slider_(lui_cutbox(lui_left,256.f),	0.f,1.f,&App.Audio.TestSignal.volume);
		lui_popbox();
		lui_popbox();

		lui_cutbox(lui_top,32.f);
		lui_popbox();

		pgraph_(lui_cutbox(lui_top,256.f),NULL,0);

	} while (lgi_tick());


	ma_device_uninit(&device);
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
		slider_(lui_bbox(24.f,lgi.Window.size_y-128.f,256,32),0,1.,&slider);


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