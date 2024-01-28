
int wdg_slider(lui_Box b, float min, float max, float *value) {
	lgi_Global float xclick;

	float xcursor = (float) lgi.Input.Mice.xcursor;
	float ycursor = (float) lgi.Input.Mice.ycursor;
	if (lui_testinbox(b,xcursor,ycursor)) {
		if (lgi_isButtonDown(0)) {
			if (!lgi_wasButtonDown(0)) {
				xclick = xcursor;
			}
			float xdelta = xcursor - b.x0;
			*value = lui_remix(xdelta,0,b.x1-b.x0-8.f,min,max);
		}
	}

	*value = lui_clip(*value,min,max);
	lui_Box t = b;
	t.x0 = lui_remix(*value,min,max,b.x0,b.x1-8.f);
	t.x1 = t.x0 + 8.f;
	// lui__drawBox(b,lgi_WHITE);
	lui__drawBox(t,lgi_BLACK);
	lui__drawBoxOutine(b,lgi_BLACK,4.f);
	lui_Box l = b;
	l.x0 = l.x1 + 12.f;
	l.x1 = l.x0 + 200.f;
	lui__drawText(l,_fmt("%.2fHz",*value));

	return 1;
}
