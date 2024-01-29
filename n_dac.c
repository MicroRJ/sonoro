
/* the whole point of this object is to get found by the
 audio device and ran on a steady interval, it acts as a bypass. */
t_node *dacconstructor(t_class *c) {
	t_dac *m = (t_dac *) baseconstructor(c, "DAC", lui_bbox(32.f,32.f,32*4,32));
	return (t_node *) m;
}

t_node *dacnode(float dac) {
	t_dac *m = (t_dac *) dacconstructor(findclass("dac"));
	return (t_node *) m;
}

/* todo: figureout how to make it so that we can still detect feedback messages,
 I think we should have a separate channel for that, and with an inlet mask telling
 us which inlet the things came from, we can update certain properties without re-triggering branch execution */
int dacmethod(t_node *n, int k) {
	/* todo: the basemethod when it receives a "CALL" message it should return the
	 number of arguments passed in */
	int result = basemethod(n,k);
	t_dac *m = (t_dac*) n;
	switch (k) {
		case CALL: {
			/* todo: also pass in an input mask that tells me which inputs were enabled */
			result = d_popint();
			if (result == 2) {
				m->enabled = d_popint();
				result = 1;
			}
		} break;
	}
	return result;
}
