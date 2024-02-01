

t_node *engineconstructor(t_class *c) {
	t_engine *m = (t_engine *) baseconstructor(c, lgi_Null, bbox(32.f,32.f,32*8,32*8));
	m->revspermin = 15.f;
	m->crankarmradius = 64.f;
	m->pistonwidth = 128.f;
	m->connectingrodlength = 16.f;
	m->crankshaftrotation = 0.;
	m->connectingrodthickness = 16;
	return (t_node *) m;
}

t_node *enginenode(float engine) {
	return engineconstructor(findclass("engine"));
}

int enginemethod(t_node *n, int k, int x, int y) {
	int result = basemethod(n,k,x,y);

	t_engine *m = (t_engine*) n;
	switch (k) {
		case CALL: {
			if (result >= 1) {
				m->revspermin = d_popfloat();
				result -= 1;
			}

			float revolutionsPerSecond = m->revspermin / 60.f;
			m->crankshaftrotation += lgi_TAU * revolutionsPerSecond * lgi.Time.deltaSeconds;

		} break;
		case DRAW: {
			lgi_Color colorBackground = lgi_RGBA_U(0xfa,0xf9,0xf6,0xff);
			lgi_Color colorMiddleground = lgi_RGBA_U(0x33,0x33,0x33,0xff);
			lgi_Color colorForeground = lgi_BLACK;

			float pistonwidth = 64.f;
			float rodlength = 128.f;
			float rodthickness = 16.f;
	/* the length from the center of the crankshaft to the center of the rod-journal */
			float crankarmlength = 16;
			float crankcaseRadius = crankarmlength * 3.f;
			float cylinderheight = crankarmlength * 2 + 8.f;
			float cylinderwallthickness = 8.f;
			float cylinderwidth = pistonwidth;

			t_box b = nodebox(n);
			float xorigin = b.x + b.zx * .5;
			float yorigin = b.y + crankcaseRadius;

			float xoscilation = cosf(m->crankshaftrotation) * crankarmlength;
			float yoscilation = sinf(m->crankshaftrotation) * crankarmlength;
			float xcrankpin = xorigin + xoscilation;
			float ycrankpin = yorigin + yoscilation;
			float xpistonpin = xorigin;
			float ypistonpin = yorigin + yoscilation + rodlength;

			/* draw piston cylinder walls */
			t_box cylinderwallbox = { 0, yorigin+rodlength-crankarmlength, cylinderwallthickness, cylinderheight };
			cylinderwallbox.x = xorigin+cylinderwidth*.5f;
			drawbox(cylinderwallbox,colorMiddleground);
			cylinderwallbox.x = xorigin-cylinderwidth*.5f-cylinderwallthickness;
			drawbox(cylinderwallbox,colorMiddleground);

			/* draw the piston */
			t_box pistonbox = {xorigin-pistonwidth*.5f,ypistonpin,pistonwidth,pistonwidth/8.f};
			drawbox(pistonbox,colorMiddleground);

			/* draw the crankarm as a circle */
			drawcircle(xorigin,yorigin,crankarmlength,colorMiddleground);
			/* draw the connecting rod */
			drawline(xcrankpin,ycrankpin,xorigin,ypistonpin,rodthickness,colorForeground);
			/* draw the pistonpin last */
			drawcircle(xpistonpin,ypistonpin,4.f,colorBackground);
			/* draw the crankpin last */
			drawcircle(xcrankpin,ycrankpin,4.f,colorBackground);
		} break;
	}
	return result;
}


