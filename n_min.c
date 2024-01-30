
t_node *minconstructor(t_class *c) {
	return baseconstructor(c, "min", bbox(32.f,32.f,32*4,32));
}

t_node *minnode() {
	return minconstructor(findclass("min"));
}

int minmethod(t_node *n, int k) {
	int result = basemethod(n,k);
	switch (k) {
		case CALL: {
			result = d_popint();
			if (result == 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				float v = b < a ? b : a;
				d_putfloat(v);
				result = 1;
			}
		} break;
	}
	return result;
}