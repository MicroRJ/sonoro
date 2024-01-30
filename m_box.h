

typedef struct t_box {
	float x, y, zx, zy;
} t_box;

t_box bbox(float x, float y, float zx, float zy) {
	return (t_box){x,y,zx,zy};
}

t_box trimbox(t_box b, float x, float y) {
	return bbox(b.x+(x*.5),b.y+(x*.5),b.zx-x,b.zy-y);
}

t_box growbox(t_box b, float x, float y) {
	return bbox(b.x-(x*.5),b.y-(x*.5),b.zx+x,b.zy+y);
}

lgi_Bool inbox(t_box b, float x, float y) {
	return ((x >= b.x) && (y >= b.y)) && ((x < b.x+b.zx) && (y < b.y+b.zy));
}

