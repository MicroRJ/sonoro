
/* the whole point of this object is to get found by the
 audio device and ran on a steady interval */
t_node *dacconstructor(t_class *c) {
	t_dac *m = (t_dac *) newnode(c, "dac", lui_bbox(32.f,32.f,32*4,32));
	return (t_node *) m;
}

t_node *dacnode(float dac) {
	t_dac *m = (t_dac *) dacconstructor(n_getclass("dac"));
	return (t_node *) m;
}

void dacmethod(t_node *n, int k) {
	t_dac *m = (t_dac*) n;
	switch (k) {
		case DSP: {
			/* discard argument count */
			d_popint();

		} break;
		case CALL: {
			int c = d_popint();
			while (c --) pop();
		} break;
		case DRAW: {
			dragnode_(n);
			drawbasenode(n);
		} break;
	}
}
