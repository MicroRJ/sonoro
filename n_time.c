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
			if (result == 1) {
				t_value restore = pop();

				lgi_longInt time = lgi_pollClock();
				double delta = (time - m->time) / (double) lgi_queryClockHz();
				m->time = time;

				for (int i=n->pclass->numoutlets-1; i>=1; i-=1) {
					t_edge outlet = n->outlets[i];
					if (outlet.target == 0) continue;

					d_putfloat(delta);
					result += 1;
				}

				rt_pushvalue(restore);
			} else {
				_log("invalid number of arguments");
			}
		} break;
	}
	return result;
}
