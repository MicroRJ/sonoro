
typedef struct t_num {
	t_node node;
	float num;
} t_num;
t_node *newnum(t_class *);
int nummethod(t_node *n, int k, int x, int y);

typedef t_node t_div;
t_node *newdiv(t_class *);
int divmethod(t_node *n, int k, int x, int y);

typedef t_node t_add;
t_node *newadd(t_class *);
int addmethod(t_node *n, int k, int x, int y);

typedef t_node t_min;
t_node *newmin(t_class *);
int minmethod(t_node *n, int k, int x, int y);


/* number */

t_node *newnum(t_class *c) {
	return (t_node *) basenew(c, lgi_Null, bbox(32.f,32.f,32*2,32));
}

int nummethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	t_num *m = (t_num*) n;
	switch (k) {
		case CALL: {
			lgi_ASSERT(result <= 1);
			if (result >= 1) {
				m->num = d_popfloat();
				result -= 1;
			}
			for (int i=0; i<n->numoutlets; i+=1) {
				d_putfloat(m->num);
				result += 1;
			}
		} break;
		case DRAW: {
			drawtext(nodebox(n),_fmt("%.2f",m->num));
		} break;
	}
	return result;
}

/* division */

t_node *newdiv(t_class *c) {
	return basenew(c, "/", bbox(32.f,32.f,32*2,32));
}

int divmethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			/* check that we have enough arguments */
			if (x >= 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				result -= 2;
				if (n->numoutlets != 0) {
					d_putfloat(b / a);
					result += 1;
				}
			}
		} break;
	}
	return result;
}

/* addition */

t_node *newadd(t_class *c) {
	return basenew(c, "+", bbox(32.f,32.f,32*2,32));
}

int addmethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			if (x >= 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				result -= 2;
				if (n->numoutlets != 0) {
					d_putfloat(b + a);
					result += 1;
				}
			}
		} break;
	}
	return result;
}

/* min */
t_node *newmin(t_class *c) {
	return basenew(c, "min", bbox(32.f,32.f,32*2,32));
}

int minmethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			if (x >= 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				result -= 2;
				if (n->numoutlets != 0) {
					d_putfloat(b < a ? b : a);
					result += 1;
				}
			}
		} break;
	}
	return result;
}