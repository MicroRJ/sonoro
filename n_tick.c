
/* the whole point of this object is to get found by the
 main loop and ran on a steady interval, it acts as a bypass. */
t_node *tickconstructor(t_class *c) {
	return basenew(c, "TICK", bbox(32.f,32.f,32*2,32));
}

/* todo: figure out how to make it so that we can still detect feedback messages,
 I think we should have a separate channel for that, and with an inlet mask telling
 us which inlet the things came from, we can update certain properties without re-triggering branch execution */
int tickmethod(t_node *n, int k, int x, int y) {
	/* todo: the basemethod when it receives a "CALL" message it should return the
	 number of arguments passed in */
	int result = basemethod(n,k,x,y);
	t_tick *m = (t_tick*) n;
	switch (k) {
		case CALL: {
			if (result >= 2) {
				t_value pass = pop();
				int enabled = d_popint();
				rt_pushvalue(pass);

				result += 1;

				m->enabled = enabled;
			}

		} break;
	}
	return result;
}
