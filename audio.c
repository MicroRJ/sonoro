


void writeTestSignal(float *output, int frameCount) {

	// double frequency = App.Audio.TestSignal.frequency;

	t_class *dacclass = findclass("dac");
	for (int i=0; i<arrlen(drawlist); i+=1) {
		t_node *n = drawlist[i];
		if (n->pclass == dacclass) {
			for (int iFrame = 0; iFrame < frameCount; iFrame += 1) {
				int results = 0; // execnode(n);
				if (results != 0) {
					float sample = (float) d_popfloat() * App.Audio.TestSignal.volume;
					for (int iChannel = 0; iChannel < CHANNELS; iChannel += 1) {
						output[iFrame*CHANNELS + iChannel] = sample;
					}
				}
			}
			break;
		}
	}
	#if 0
	// _log(lgi_INFO,_fmt("Frame Count %i",frameCount));
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
	#endif
}

void audioproc(ma_device* device, void *output, const void *input, ma_uint32 frameCount) {
	writeTestSignal(output,frameCount);


	// if (output != NULL && input != NULL) {
	// 	MA_COPY_MEMORY(output,input,frameCount*ma_get_bytes_per_frame(device->capture.format,device->capture.channels));
	// }
}


void audioend() {
	ma_device_uninit(&App.Audio.Device.handle);
}

void audiobegin() {
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
	config.dataCallback       = audioproc;
	config.pUserData          = NULL;

	if (ma_device_init(&App.Audio.context, &config, &App.Audio.Device.handle) != MA_SUCCESS) {
		return;
	}

	ma_device_start(&App.Audio.Device.handle);
}