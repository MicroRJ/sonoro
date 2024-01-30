
t_node *toggleconstructor(t_class *c) {
	return baseconstructor(c, lgi_Null, lui_bbox(32.f,32.f,32,32));
}

int togglemethod(t_node *n, int k) {
	int result = basemethod(n,k);

	t_toggle *m = (t_toggle*) n;
	switch (k) {
		case CALL: {
			int c = d_popint();
			if (c == 1) {
				m->toggled = d_popint();
			}
			d_putint(m->toggled);
			result = 1;
		} break;
		case DRAW: {
			if (!result) {
				if (clicked(nodebox(n))) {
					m->toggled = !m->toggled;
					notify(n);
				}
			}
			lgi_Color color = UI_COLOR_FOREGROUND;
			if(!m->toggled) {
				color = UI_COLOR_FADED;
			}
			lui__drawBox(lui_Box_shrink(nodebox(n),8.f,8.f),color);
		} break;
	}
	return result;
}
