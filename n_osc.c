
t_node *oscconstructor(t_class *c) {
	return newnode(c, lgi_Null, lui_bbox(32.f,32.f,32*4,32));
}

t_node *oscnode(float hz) {
	t_osc *m = (t_osc *) oscconstructor(n_getclass("osc"));
	m->hz = hz;
	return (t_node *) m;
}

void oscmethod(t_node *n, int k) {
	t_osc *m = (t_osc*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			if (c == 1) {
				m->hz = popf();
			} else while (c --) pop();

			d_putfloat(sinf(lgi_TAU * m->time));
			m->time += 1.f / (SAMPLE_RATE / m->hz);
		} break;
		case DRAW: {
			dragnode_(n);
			drawbasenode(n);
			lui__drawText(nodebox(n),_fmt("osc~ %.2f",m->hz));
		} break;
	}
}

