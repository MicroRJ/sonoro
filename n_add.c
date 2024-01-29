
t_node *addconstructor(t_class *c) {
	return baseconstructor(c, "add", lui_bbox(32.f,32.f,32*4,32));
}

int addmethod(t_node *n, int k) {
	int result = basemethod(n,k);
	switch (k) {
		case CALL: {
			/* check that we have enough arguments */
			int c = d_popint();
			if (c == 2) {
				float a = d_popfloat();
				float b = d_popfloat();
				float v = b + a;
				// _log("add %f + %f = %f",a,b,v);
				d_putfloat(v);
				result = 1;
			} else {
				_log("invalid number of arguments");
			}
		} break;
	}
	return result;
}