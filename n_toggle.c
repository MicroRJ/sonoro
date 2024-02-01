
t_node *toggleconstructor(t_class *c) {
	return baseconstructor(c, lgi_Null, bbox(32.f,32.f,32,32));
}

int togglemethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);

	t_toggle *m = (t_toggle*) n;
	switch (k) {
		case CALL: {
			if (result >= 1) {
				m->toggled = d_popint();
			}
			d_putint(m->toggled);
			result += 1;
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
			drawbox(trimbox(nodebox(n),8.f,8.f),color);
		} break;
	}
	return result;
}
