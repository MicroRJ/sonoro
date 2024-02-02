/* timer() -> double
 returns the timer elapsed between two calls */

typedef struct t_timer {
	t_node node;
	lgi_longInt time;
} t_timer;
t_node *timerconstructor(t_class *);
int timermethod(t_node *n, int k, int x, int y);


t_node *timerconstructor(t_class *c) {
	t_timer *timer = (t_timer *) basenew(c, "timer", bbox(32.f,32.f,32*3,32));
	timer->time = lgi_pollClock();
	return (t_node *) timer;
}

int timermethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	t_timer *m = (t_timer*) n;
	switch (k) {
		case CALL: {
			lgi_longInt time = lgi_pollClock();
			double delta = (time - m->time) / (double) lgi_queryClockHz();
			m->time = time;

			t_value passthrough = (t_value){VERROR};
			if (n->numinlets == 0) {
				/* our first outlet contains the pass through value,
				if there are no inlets, we make our own.. */
				rt_pushvalue(passthrough);
				// if (n->outlets[0].target != 0) {
				// }
			} else {
				// if (n->outlet[i].target == 0) {
				// 	pop();
				// } else
				// if (n->outlet[i].type == RED_WIRE) {
				// 	pop();
				// }
			}
			for (int i=n->pclass->numoutlets-1; i>=1; i-=1) {
				t_edge outlet = n->outlets[i];
				if (outlet.target == 0) continue;
				if (outlet.type == RED_WIRE) continue;

				d_putfloat(delta);
				result += 1;
				_log("%f",delta);
			}
		} break;
	}
	return result;
}
