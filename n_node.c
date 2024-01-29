
t_node *seldragnode;
t_node *selinletnode;
int selinletslot;
int dragnodexclick;
int dragnodeyclick;
int lastnodeid;

/* remove from here */
t_node **drawlist;


t_node *baseconstructor(t_class *pclass, char const *label, lui_Box box) {
	t_node *n = calloc(1,pclass->typesize);
	n->id = lastnodeid ++;
	n->pclass = pclass;
	n->label = label;
	n->box = box;
	return n;
}

void exportnode(t_node *n, t_exporter *e) {
	if (n->pclass->export_ != 0) {
		n->pclass->export_(n,e);
	} else {
		exportbasenode(n,e);
	}
}

t_node *importnode(t_class *c, t_importer *i) {
	return c->import_ != 0 ? c->import_(c,i) : importbasenode(c,i);
}

void drawnode(t_node *n) {
	n->pclass->method(n,DRAW);
}

int execnode(t_node *n) {
	return n->pclass->method(n,EXEC);
}

void notify(t_node *n) {
	if (n->outlets == 0 && n->inlets != 0) {
		_log("notify: %s",n->pclass->name);
		// __debugbreak();
		execnode(n);
		return;
	}

	for (t_edge *e = n->outlets; e != 0; e = e->list) {
		notify(e->target);
	}
}


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




void n_addinlet(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot) {
	n_addoutlet(outletnode,outletslot,inletnode,inletslot);
}


void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot) {
	if (outletslot >= outletnode->pclass->numoutlets) {
		_log("too many outlets");
	}
	if (inletslot >= inletnode->pclass->numinlets) {
		_log("too many inlets");
	}
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



