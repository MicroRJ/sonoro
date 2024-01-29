
t_node *oscconstructor(t_class *c) {
	return baseconstructor(c, lgi_Null, lui_bbox(32.f,32.f,32*4,32));
}

t_node *oscnode(float hz) {
	t_osc *m = (t_osc *) oscconstructor(findclass("osc"));
	m->hz = hz;
	return (t_node *) m;
}

int oscmethod(t_node *n, int k) {
	int result = basemethod(n,k);
	t_osc *m = (t_osc*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			if (c == 1) {
				m->hz = d_popfloat();
			}

			d_putfloat((float) (sin(lgi_TAU * m->time)));
			m->time += 1. / (SAMPLE_RATE / m->hz);
			result = 1;
		} break;
		case DRAW: {
			lui__drawText(nodebox(n),_fmt("osc~ %.2f",m->hz));
		} break;
	}

	return result;
}

