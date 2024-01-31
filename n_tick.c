
/* the whole point of this object is to get found by the
 main loop and ran on a steady interval, it acts as a bypass. */
t_node *tickconstructor(t_class *c) {
	return baseconstructor(c, "TICK", bbox(32.f,32.f,32*2,32));
}

/* todo: figure out how to make it so that we can still detect feedback messages,
 I think we should have a separate channel for that, and with an inlet mask telling
 us which inlet the things came from, we can update certain properties without re-triggering branch execution */
int tickmethod(t_node *n, int k) {
	/* todo: the basemethod when it receives a "CALL" message it should return the
	 number of arguments passed in */
	int result = basemethod(n,k);
	t_tick *m = (t_tick*) n;
	switch (k) {
		case CALL: {
			/* todo: also pass in an input mask that tells me which inputs were enabled */
			result = d_popint();
			if (result == 2) {
				float f = d_popfloat();
				m->enabled = d_popint();
				d_putfloat(f);
				result = 1;
			}

		} break;
	}
	return result;
}