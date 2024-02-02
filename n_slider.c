
t_node *sliderconstructor(t_class *c) {
	t_slider *slider = (t_slider *) basenew(c, lgi_Null, bbox(32.f,32.f,32*6,32));
	slider->max = 880;
	slider->min = 0;
	slider->val = 440;
	return (t_node *) slider;
}

int slidermethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);
	t_slider *s = (t_slider *) n;
	switch (k) {
		case CALL: {
			if (result >= 1) {
				s->val = d_popfloat();
				s->val = lui_clip(s->val,s->min,s->max);
				result -= 1;
			}
			if (n->numoutlets != 0) {
				d_putfloat(s->val);
				result += 1;
			}
		} break;
		case DRAW: {
			t_box b = nodebox(n);

			if (!result) {
				lgi_Global float xclick;
				float xcursor = (float) getxcursor();
				float ycursor = (float) getycursor();
				if (inbox(b,xcursor,ycursor)) {
					if (lgi_isButtonDown(0)) {
						if (!lgi_wasButtonDown(0)) {
							xclick = xcursor;
						}
						s->val = lui_remix(xcursor-b.x,0,b.zx-8.f,s->min,s->max);
						notify(n);
					}
				}

			}

			s->val = lui_clip(s->val,s->min,s->max);

			/* draw the thumb */
			t_box t = b;
			t.x = lui_remix(s->val,s->min,s->max,b.x,b.x+b.zx-8.f);
			t.zx = 8.f;

			drawbox(t,lgi_BLACK);

			#if 0
			lui_Box l = b;
			l.x0 = l.x1 + 12.f;
			l.x1 = l.x0 + 200.f;
			drawtext(l,_fmt("%.2fHz",s->val));
			#endif
		} break;
	}
	return result;
}

void sliderexportmethod(t_node *n, t_unloader *e) {
	t_slider *slider = (t_slider *) n;
	unloadbasenode(n,e);
	d_putfloat(slider->min); ini_writefield(e,"min");
	d_putfloat(slider->max); ini_writefield(e,"max");
	d_putfloat(slider->val); ini_writefield(e,"val");
}
t_node *sliderimportmethod(t_class *c, t_loader *i) {
	t_slider *slider = (t_slider *) loadbasenode(c,i);
	char b[MAX_NAME];
	while (ini_nextfield(i,b)) {
		if (!strcmp(b,"min")) slider->min = d_popfloat(); else
		if (!strcmp(b,"max")) slider->max = d_popfloat(); else
		if (!strcmp(b,"val")) slider->val = d_popfloat(); else
		_log("no such field");
	}
	return (t_node *) slider;
}


