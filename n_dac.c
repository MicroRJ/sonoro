
/* the whole point of this object is to get found by the
 audio device and ran on a steady interval, it acts as a bypass. */
t_node *dacconstructor(t_class *c) {
	t_dac *m = (t_dac *) baseconstructor(c, "dac", lui_bbox(32.f,32.f,32*4,32));
	return (t_node *) m;
}

t_node *dacnode(float dac) {
	t_dac *m = (t_dac *) dacconstructor(findclass("dac"));
	return (t_node *) m;
}

int dacmethod(t_node *n, int k) {
	/* todo: the basemethod when it receives a "CALL" message it should return the
	 number of arguments passed in */
	int result = basemethod(n,k);
	t_dac *m = (t_dac*) n;
	switch (k) {
		case CALL: {
			result = d_popint();
		} break;
	}
	return result;
}
