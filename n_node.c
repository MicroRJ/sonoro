
/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */

typedef lui_Box t_box;
typedef struct t_node t_node;
typedef struct t_edge t_edge;

enum {
	VFLOAT,
	VINT,
};
typedef struct t_value {
	char k;
	union {
		float f;
		int i;
	};
} t_value;

int check();
float popf();
int popi();
void pop();

void putf(float x);
void puti(int x);

typedef struct t_edge {
	t_edge *list;
	t_node *target;
	int outletslot,inletslot;
} t_edge;

enum {
	CALL,
	DRAW,
};

typedef void (t_proc) (t_node *n, int k);

typedef struct t_node {
	char const *label;
	t_proc *proc;
	t_edge *outlets;
	t_edge *inlets;
	int numinlets;
	int numoutlets;
	t_box box;
} t_node;

void drawnode(t_node *n);
void callnode(t_node *n, int c);
void notify(t_node *n);
void execnode(t_node *n);
void initnode(t_node *n, char const *label, t_proc *proc, int numinlets, int numoutlets, lui_Box box);
void addedge(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot);


t_value stack[0x1000];
int istack;
t_node *seldragnode;
t_node *selinletnode;
int selinletslot;
t_node **drawlist;
int dragnodexclick;
int dragnodeyclick;

t_box nodebox(t_node *n) {
	t_box b = n->box;
	/* replace with flag - todo */
	if (seldragnode == n) {
		int xdelta = lgi.Input.Mice.xcursor - dragnodexclick;
		int ydelta = lgi.Input.Mice.ycursor - dragnodeyclick;
		b.x0 += xdelta; b.x1 += xdelta;
		b.y0 += ydelta; b.y1 += ydelta;
	}
	return b;
}

void exec() {
	_log("running!");

	for (int i=0; i<arrlen(drawlist); i+=1) {
		t_node *n = drawlist[i];
		/* find end of execution branch */
		if (n->inlets != 0 && n->outlets == 0) {
			execnode(n);
		}
	}
}

void addnode(t_node *n) {
	*arradd(drawlist,1) = n;
}




void putf(float f) {
	stack[istack].k = VINT;
	stack[istack].f = f;
	istack += 1;
}
float popf() {
	return stack[-- istack].f;
}
void puti(int i) {
	stack[istack].k = VINT;
	stack[istack].i = i;
	istack += 1;
}
void pop() {
	-- istack;
}
int popi() {
	return stack[-- istack].i;
}



void drawnode(t_node *n) {
	n->proc(n,DRAW);
}
void callnode(t_node *n, int c) {
	puti(c);
	n->proc(n,CALL);
	if (n->outlets == NULL) {
		for (int i=0;i<n->numoutlets;i+=1){
			pop();
		}
	}
}

void notify(t_node *n) {
	if (n->outlets == 0 && n->inlets != 0) {
		execnode(n);
		return;
	}

	for (t_edge *e = n->outlets; e != 0; e = e->list) {
		notify(e->target);
	}
}

/* Calls all the inlets of the node first, then calls the node itself */
void execnode(t_node *n) {
	int c = 0;
	for (t_edge *e = n->inlets; e != 0; e = e->list) {
		execnode(e->target);
		c += 1;
	}

	callnode(n,c);
}

void initnode(t_node *n, char const *label, t_proc *proc, int numinlets, int numoutlets, lui_Box box) {
	memset(n,0,sizeof(*n));
	n->label = label;
	n->proc = proc;
	n->numinlets = numinlets;
	n->numoutlets = numoutlets;
	n->box = box;
}

void addedge(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot) {
	/* outlet, connects outletnode to inlet node */
	t_edge *outlet = malloc(sizeof(t_edge));
	outlet->list = outletnode->outlets;
	outlet->target = inletnode;
	outlet->inletslot = inletslot;
	outlet->outletslot = outletslot;
	outletnode->outlets = outlet;

	/* inlet, connects inletnode to outlet node */
	t_edge *inlet = malloc(sizeof(t_edge));
	inlet->list = inletnode->inlets;
	inlet->target = outletnode;
	inlet->inletslot = inletslot;
	inlet->outletslot = outletslot;
	inletnode->inlets = inlet;
}


lgi_Bool clicked(lui_Box box) {
	float xcursor = (float) lgi.Input.Mice.xcursor;
	float ycursor = (float) lgi.Input.Mice.ycursor;
	return lgi_isButtonPressed(0) && lui_testinbox(box,xcursor,ycursor);
}

t_box getinletbox(t_box b, int i) {
	return lui_bbox(b.x0+i*16+i*8,b.y1+4,16,8);
}

t_box getoutletbox(t_box b, int i) {
	return lui_bbox(b.x0+i*16+i*8,b.y0-8-4,16,8);
}

int dragnode_(t_node *n) {
	lui_Box b = nodebox(n);

	if (lgi_testCtrlKey()) {
		/* make it a little bit easier to click */
		if (clicked(lui_Box_enlarge(b,32.f,32.f))) {
			for (int i=0; i<n->numinlets; i+=1) {
				if (clicked(lui_Box_enlarge(getinletbox(b,i),8.f,8.f))) {
					selinletnode = n;
					selinletslot = i;
					return lgi_True;
				}
			}
			if (clicked(b)) {
				dragnodexclick = lgi.Input.Mice.xcursor;
				dragnodeyclick = lgi.Input.Mice.ycursor;
				seldragnode = n;
			}
		}
	}

	if (lgi_isButtonReleased(0)) {
		if (seldragnode == n) {
			n->box = nodebox(n);
			seldragnode = lgi_Null;
		}
		if (selinletnode == n) {
			selinletnode = lgi_Null;
			selinletslot = 0;
		}
	}
	return seldragnode == n;
}

void drawnode_(t_node *n) {
	lui_Box b = nodebox(n);

	lgi_Color color = UI_COLOR_FOREGROUND;
	drawboxoutline(b,4.f,color);

	for (int i=0; i<n->numinlets; i+=1) {
		lui__drawBox(getinletbox(b,i),UI_COLOR_FOREGROUND);
	}
	for (int i=0; i<n->numoutlets; i+=1) {
		lui__drawBox(getoutletbox(b,i),UI_COLOR_FOREGROUND);
	}

	for (t_edge *e = n->outlets; e != 0; e = e->list) {
		t_box o = getoutletbox(b,e->outletslot);
		t_box i = getinletbox(nodebox(e->target),e->inletslot);
		lgi_drawLine(UI_COLOR_FOREGROUND,4.f
		,	o.x0+(o.x1-o.x0)*.5f,o.y0+(o.y1-o.y0)*.5f
		,	i.x0+(i.x1-i.x0)*.5f,i.y0+(i.y1-i.y0)*.5f);
	}
	if (n->label != 0) {
		lui__drawText(b,n->label);
	}
}

