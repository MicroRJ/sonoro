
t_node *numconstructor(t_class *c) {
	t_num *m = (t_num *) baseconstructor(c, lgi_Null, bbox(32.f,32.f,32*4,32));
	m->num = 0.f;
	return (t_node *) m;
}

t_node *numnode(float num) {
	t_num *m = (t_num *) numconstructor(findclass("num"));
	m->num = num;
	return (t_node *) m;
}

int nummethod(t_node *n, int k) {
	int result = basemethod(n,k);

	t_num *m = (t_num*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			if (c == 1) {
				m->num = d_popfloat();
			}
			d_putfloat(m->num);
			result = 1;
		} break;
		case DRAW: {
			drawtext(nodebox(n),_fmt("%.2f",m->num));
		} break;
	}
	return result;
}
