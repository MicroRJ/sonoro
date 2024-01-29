
t_node *minconstructor(t_class *c) {
	return newnode(c, "min", lui_bbox(32.f,32.f,32*4,32));
}

t_node *minnode() {
	return minconstructor(n_getclass("min"));
}

void minmethod(t_node *n, int k) {
	switch (k) {
		case CALL: {
			/* check that we have enough arguments */
			int c = d_popint();
			if (c == 2) {
				float a = popf();
				float b = popf();
				float v = b < a ? b : a;
				d_putfloat(v);
			} else {
				_log("invalid number of arguments");
			}
		} break;
		case DRAW: {
			dragnode_(n);
			drawbasenode(n);
		} break;
	}
}