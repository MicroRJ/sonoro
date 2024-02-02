
/* splitter */
typedef t_node t_splitter;
t_node *splitterconstructor(t_class *);
int splittermethod(t_node *n, int k, int x, int y);

t_node *splitterconstructor(t_class *c) {
	return basenew(c, "split", bbox(32.f,32.f,32*2,32));
}

int splittermethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			if (result >= 1) {
				for (int i=0; i<n->pclass->numoutlets; i+=1) {
					t_edge outlet = n->outlets[i];
					if (outlet.target == 0) continue;
					d_dup(1);
					result += 1;
				}
			}
		} break;
		case DRAW: {
		} break;
	}
	return result;
}



/* detector */
typedef t_node t_detector;
t_node *detectorconstructor(t_class *);
int detectormethod(t_node *n, int k, int x, int y);

t_node *detectorconstructor(t_class *c) {
	return basenew(c, lgi_Null, bbox(32.f,32.f,32*3,32));
}

int detectormethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			/* Generate an integer for each outlet we have active,
			the first outlet is the one that drives the execution
			flow upwards, so we don't generate one for that one. */
			for (int i=n->pclass->numoutlets-1; i>=1; i-=1) {
				if (n->outlets[i].target == 0) continue;
				d_putint(1);
				result += 1;
			}
		} break;
		case DRAW: {
		} break;
	}
	return result;
}
