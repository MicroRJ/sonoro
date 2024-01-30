

t_node *engineconstructor(t_class *c) {
	t_engine *m = (t_engine *) baseconstructor(c, lgi_Null, lui_bbox(32.f,32.f,32*8,32*8));
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

int enginemethod(t_node *n, int k) {
	int result = basemethod(n,k);

	t_engine *m = (t_engine*) n;
	switch (k) {
		case CALL: {
			result = d_popint();
		} break;
		case DRAW: {
			lgi_Color colorBackground = lgi_RGBA_U(0xfa,0xf9,0xf6,0xff);
			lgi_Color colorMiddleground = lgi_RGBA_U(0x33,0x33,0x33,0xff);
			lgi_Color colorForeground = lgi_BLACK;

			float revolutionsPerMinute = 15.f;
			double crankshaftRotation = 0.;
			float pistonWidth = 64.f;
			float connectingRodLength = 128.f;
			float connectingRodThickness = 16.f;
	/* the length from the center of the crankshaft to the center of the rod-journal */
			float crankarmRadius = 16;
			float crankcaseRadius = crankarmRadius * 3.f;
			float engineBlockHeight = crankarmRadius * 2 + 8.f;
			float engineBlockThickness = 8.f;
			float pistonCylinderWidth = pistonWidth;

			t_box b = nodebox(n);
			float xorigin = b.x0 + (b.x1 - b.x0) * .5;
			float yorigin = b.y0 + crankcaseRadius;
			vec2 center = vec2_xy(xorigin,yorigin);

			// wdg_slider(lui_bbox(8.f,8.f+32*0,256,32),0,500,&revolutionsPerMinute);
			// wdg_slider(lui_bbox(8.f,8.f+32*1,256,32),64,256,&connectingRodLength);
			// wdg_slider(lui_bbox(8.f,8.f+32*2,256,32),4,256,&crankarmRadius);
			// wdg_slider(lui_bbox(8.f,8.f+32*3,256,32),4,256,&pistonWidth);



			float xoscilation = cosf(crankshaftRotation) * crankarmRadius;
			float yoscilation = sinf(crankshaftRotation) * crankarmRadius;
			float xcrankpin = xorigin + xoscilation;
			float ycrankpin = yorigin + yoscilation;
			float xpistonpin = xorigin;
			float ypistonpin = yorigin + yoscilation + connectingRodLength;

			float revolutionsPerSecond = revolutionsPerMinute / 60.f;
			crankshaftRotation += lgi_TAU * revolutionsPerSecond * lgi.Time.deltaSeconds;

		// lgi_drawBox(vec2_xy(xorigin,yorigin),vec2_xy(32.f,8.f),colorMiddleground,0.f,0.f);

		/* draw piston cylinder */
			lgi_drawBox(vec2_xy(xorigin-pistonCylinderWidth*.5f-engineBlockThickness,yorigin+connectingRodLength- crankarmRadius),vec2_xy(engineBlockThickness,engineBlockHeight),colorMiddleground,0.f,0.f);
			lgi_drawBox(vec2_xy(xorigin+pistonCylinderWidth*.5f,yorigin+connectingRodLength- crankarmRadius),vec2_xy(engineBlockThickness,engineBlockHeight),colorMiddleground,0.f,0.f);



		// lgi_drawBox(vec2_xy(xorigin-pistonCylinderWidth*.5f,connectingRodLength+100.f),vec2_xy(pistonCylinderWidth,engineBlockHeight),colorMiddleground,0.f,0.f);

		/* draw the piston */
			lgi_drawBox(vec2_xy(xorigin-pistonWidth*.5f,ypistonpin),vec2_xy(pistonWidth,pistonWidth/8.f),colorMiddleground,0.f,0.f);
		/* draw the crankshaft*/
			lgi_drawCircleSDF(center,vec2_xy(crankarmRadius,crankarmRadius),colorMiddleground,0.f,2.f);
		/* draw the connecting rod */
			lgi_drawLine(colorForeground,connectingRodThickness
			, xcrankpin,ycrankpin, xorigin, ypistonpin);
		/* draw the pistonpin last */
			lgi_drawCircleSDF(vec2_xy(xpistonpin,ypistonpin),vec2_xy(4.f,4.f),colorBackground,0.f,2.f);
		/* draw the crankpin last */
			lgi_drawCircleSDF(vec2_xy(xcrankpin,ycrankpin),vec2_xy(4.f,4.f),colorBackground,0.f,2.f);
		} break;
	}
	return result;
}


