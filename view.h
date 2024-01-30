
lgi_Global float xoffset;
lgi_Global float yoffset;

int getxcursor() {
	return lgi.Input.Mice.xcursor - xoffset;
}

int getycursor() {
	return lgi.Input.Mice.ycursor - yoffset;
}

lgi_Bool clicked(t_box b) {
	return lgi_isButtonPressed(0) && inbox(b,getxcursor(),getycursor());
}

void drawtext(t_box b, char const *string) {
	lui__drawText((lui_Box){xoffset + b.x,yoffset + b.y,xoffset + b.x+b.zx,yoffset + b.y+b.zy},string);
}

void drawbox(t_box b, lgi_Color c) {
	lgi_drawQuad(c,xoffset + b.x,yoffset + b.y,b.zx,b.zy);
}

void drawcircle(float x, float y, float r, lgi_Color c) {
	lgi_drawCircleSDF((vec2){xoffset + x,yoffset + y},(vec2){r,r},c,1,1);
}

void drawline(float x0, float y0, float x1, float y1, float t, lgi_Color c) {
	lgi_drawLine(c,t, xoffset + x0, yoffset + y0, xoffset + x1, yoffset + y1);
}

lgi_API void drawboxoutline(t_box b, float thickness, lgi_Color color) {
	float x0 = xoffset + b.x - thickness;
	float y0 = yoffset + b.y - thickness;
	float x1 = xoffset + b.x + b.zx;
	float y1 = yoffset + b.y + b.zy;

	lgi_drawQuad(color,x0,y1,b.zx+thickness*2,thickness);
	lgi_drawQuad(color,x0,y0,b.zx+thickness*2,thickness);

	lgi_drawQuad(color,x0,yoffset+b.y,thickness,b.zy);
	lgi_drawQuad(color,x1,yoffset+b.y,thickness,b.zy);
}
