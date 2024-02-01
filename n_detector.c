
t_node *detectorconstructor(t_class *c) {
	return baseconstructor(c, lgi_Null, bbox(32.f,32.f,32*3,32));
}

int detectormethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	switch (k) {
		case CALL: {
			/* generate several integers to indicate this thing has been
			triggered, or called. it should be a message for n-outlets - 1
			since the first outlet does not receive anything. */
			for (int i=0; i<arrlen(n->outlets)-1; i+=1) {
				d_putint(1);
				result += 1;
			}
		} break;
		case DRAW: {
		} break;
	}
	return result;
}
