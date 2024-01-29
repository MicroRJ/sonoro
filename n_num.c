
t_node *numconstructor(t_class *c) {
	t_num *m = (t_num *) newnode(c, lgi_Null, lui_bbox(32.f,32.f,32*4,32));
	m->num = 0.f;
	return (t_node *) m;
}

t_node *numnode(float num) {
	t_num *m = (t_num *) numconstructor(n_getclass("num"));
	m->num = num;
	return (t_node *) m;
}

void nummethod(t_node *n, int k) {
	t_num *m = (t_num*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			if (c == 1) {
				m->num = popf();
			}
			d_putfloat(m->num);
		} break;
		case DRAW: {
			dragnode_(n);
			drawbasenode(n);
			lui__drawText(nodebox(n),_fmt("%.2f",m->num));
		} break;
	}
}
