
t_node *graphconstructor(t_class *c) {
	t_graph *g = (t_graph *) baseconstructor(c, lgi_Null, bbox(32.f,32.f,32*8,32*4));
	g->length = 1024;
	g->buffer = calloc(1024,sizeof(float));
	return (t_node *) g;
}

int graphmethod(t_node *n, int k) {
	int result = basemethod(n,k);
	t_graph *m = (t_graph*) n;
	switch (k) {
		case CALL: {
			result = d_popint();
			if (result != 0) {
				if (m->cursor >= m->length) {
					m->cursor = 0;
				}
				float f = d_popfloat();
				m->buffer[m->cursor ++] = f;
				d_putfloat(f);
			}
		} break;
		case DRAW: {
			t_box b = nodebox(n);
			float p = b.zx / m->length;
			for (int i=0; i<m->length; i+=1) {
				float y = b.y + lui_remix(m->buffer[i],-2.f,+2.f,0,b.zy);
				float x = b.x + i*p;
				drawcircle(x,y,p,UI_COLOR_FOREGROUND);
			}
		} break;
	}

	return result;
}



