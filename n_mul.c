
t_node *divconstructor(t_class *c) {
	return baseconstructor(c, "div", bbox(32.f,32.f,32*4,32));
}

int divmethod(t_node *n, int k) {
	int result = basemethod(n,k);
	switch (k) {
		case CALL: {
			/* check that we have enough arguments */
			result = d_popint();
			if (result == 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				float v = b / a;
				d_putfloat(v);
				result = 1;
			}
		} break;
	}
	return result;
}