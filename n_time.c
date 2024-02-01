/* timer() -> double
 returns the timer elapsed between two calls */

t_node *timerconstructor(t_class *c) {
	t_timer *timer = (t_timer *) baseconstructor(c, "timer", bbox(32.f,32.f,32*3,32));
	timer->time = lgi.Time.totalSeconds;
	return (t_node *) timer;
}

int timermethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	t_timer *m = (t_timer*) n;
	switch (k) {
		case CALL: {
			if (n->outlets != 0) {
				double delta = lgi.Time.totalSeconds - m->time;
				d_putfloat(delta);
				result += 1;
			}
		} break;
	}
	return result;
}
