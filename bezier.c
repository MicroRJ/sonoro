
typedef struct t_point {
	float x, y;
} t_point;

// B(t) = p0 + t(p1-p0)
t_point bzline(float t, t_point p0, t_point p1) {
	return (t_point) {
		p0.x + (p1.x - p0.x) * t,
		p0.y + (p1.y - p0.y) * t,
	};
}

t_point bzquad(float t, t_point p0, t_point p1, t_point p2) {
	return bzline(t,bzline(t,p0,p1),bzline(t,p1,p2));
}

t_point bzcube(float t, t_point p0, t_point p1, t_point p2, t_point p3) {
	return bzline(t,bzquad(t,p0,p1,p2),bzquad(t,p1,p2,p3));
}