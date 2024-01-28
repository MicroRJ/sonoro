

typedef struct t_osc {
	t_node node;
	float hz;
	double time;
} t_osc;

void oscproc(t_node *n, int k) {
	t_osc *m = (t_osc*) n;
	switch (k) {
		case CALL: {
			int c = popi();
			if (c == 1) {
				m->hz = popf();
			} else while (c --) pop();

			putf(sinf(lgi_TAU * m->time));
			m->time += 1.f / (SAMPLE_RATE / m->hz);
		} break;
		case DRAW: {
			dragnode_(n);
			drawnode_(n);
			lui__drawText(nodebox(n),_fmt("osc~ %.2f",m->hz));
		} break;
	}
}

t_node *oscnode(float hz) {
	t_osc *m = malloc(sizeof(t_osc));
	m->hz = hz;
	initnode((t_node*) m, lgi_Null, oscproc, 1, 1, lui_bbox(32.f,32.f,32*6,32));
	return (t_node *) m;
}
