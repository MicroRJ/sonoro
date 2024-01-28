
typedef struct t_num {
	t_node node;
	float num;
} t_num;

typedef struct t_min {
	t_node node;
} t_min;

void numproc(t_node *n, int k) {
	t_num *m = (t_num*) n;
	switch (k) {
		case CALL: {
			int c = popi();
			if (c == 1) {
				m->num = popf();
			}
			putf(m->num);
		} break;
		case DRAW: {
			dragnode_(n);
			drawnode_(n);
			lui__drawText(nodebox(n),_fmt("%.2f",m->num));
		} break;
	}
}

t_node *numnode(float num) {
	t_num *m = malloc(sizeof(t_num));
	m->num = num;
	initnode((t_node*) m, lgi_Null, numproc, 1, 1, lui_bbox(32.f,32.f,32*4,32));
	return (t_node *) m;
}

void minproc(t_node *n, int k) {
	switch (k) {
		case CALL: {
			/* check that we have enough arguments */
			int c = popi();
			if (c == 2) {
				float a = popf();
				float b = popf();
				float v = b < a ? b : a;
				putf(v);
			} else {
				lgi_logInfo("invalid number of arguments, %i, expected %i", c, 2);
			}
		} break;
		case DRAW: {
			dragnode_(n);
			drawnode_(n);
		} break;
	}
}

t_node *minnode() {
	t_min *m = malloc(sizeof(t_min));
	initnode((t_node*) m, "min", minproc, 2, 1, lui_bbox(32.f,32.f,32*4,32));
	return (t_node*) m;
}