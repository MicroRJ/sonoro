
t_node *oscconstructor(t_class *c) {
	return basenew(c, lgi_Null, bbox(32.f,32.f,32*4,32));
}

int oscmethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	t_osc *m = (t_osc*) n;
	switch (k) {
		case CALL: {
			if (result >= 1) {
				m->hz = d_popfloat();
				result -= 1;
			}

			d_putfloat((float) (sin(lgi_TAU * m->time)));
			result += 1;

			m->time += 1. / (SAMPLE_RATE / m->hz);
		} break;
		case DRAW: {
			drawtext(nodebox(n),_fmt("osc~ %.2f",m->hz));
		} break;
	}

	return result;
}

