
t_node *graphconstructor(t_class *c) {
	return newnode(c, lgi_Null, lui_bbox(32.f,32.f,32*8,32*4));
}

t_node *graphnode(char const *label, int samples) {
	t_graph *m = (t_graph *) graphconstructor(n_getclass("graph"));
	m->samples = samples;
	return (t_node *) m;
}

void graphmethod(t_node *n, int k) {
	t_graph *m = (t_graph*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			while (c --) pop();
		} break;
		case DRAW: {
			dragnode_(n);
			drawbasenode(n);

			t_box b = nodebox(n);
			float window = b.x1 - b.x0;
			int resolution = window;
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
			}
			globalTime += lgi.Time.deltaSeconds;
		} break;
	}
}



