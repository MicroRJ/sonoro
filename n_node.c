
t_node *seldragnode;
t_node *selinletnode;
int selinletslot;
int dragnodexclick;
int dragnodeyclick;
int lastnodeid;

/* remove from here */
t_node **drawlist;


t_node *n_newnode(t_class *c) {
	return c->constructor(c);
}

t_class *n_getclass(char const *name) {
	for (int i = 0; i < arrlen(module); i += 1) {
		t_class *c = module[i];
		if (!strcmp(c->name,name)) {
			return c;
		}
	}
	return 0;
}

t_class *n_newclass(char const *name, int typesize, int numinlets, int numoutlets, t_constructor *constructor, t_method *method, t_exportmethod *mexport, t_importmethod *mimport) {
	t_class *c = calloc(1,sizeof(t_class));
	strcpy(c->name,name);
	c->typesize = typesize;
	c->numinlets = numinlets;
	c->numoutlets = numoutlets;
	c->constructor = constructor;
	c->method = method;
	c->export_ = mexport;
	c->import_ = mimport;

	*arradd(module,1) = c;
	return c;
}

void n_moduleinit() {
	n_newclass( "num", sizeof(t_num), 1, 1, numconstructor, nummethod, 0, 0);
	n_newclass( "min", sizeof(t_min), 2, 1, minconstructor, minmethod, 0, 0);
	n_newclass( "osc", sizeof(t_osc), 1, 1, oscconstructor, oscmethod, 0, 0);
	n_newclass( "slider", sizeof(t_slider), 1, 1, sliderconstructor, slidermethod, sliderexportmethod, sliderimportmethod );
	n_newclass( "graph", sizeof(t_graph), 1, 0, graphconstructor, graphmethod, 0, 0);
	n_newclass( "dac", sizeof(t_dac), 1, 0, dacconstructor, dacmethod, 0, 0);
}


t_node *ininode(t_class *pclass) {
	t_node *n = calloc(1,pclass->typesize);
	n->pclass = pclass;
	return n;
}

t_node *newnode(t_class *pclass, char const *label, lui_Box box) {
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

void callnode(t_node *n, int c) {
	d_putint(c);
	n->pclass->method(n,CALL);
	if (n->outlets == NULL) {
		for (int i=0;i<n->pclass->numoutlets;i+=1){
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


/* Calls all the inlets of the node first, then calls the node itself */
void execnode(t_node *n) {
	int c = 0;
	for (t_edge *e = n->inlets; e != 0; e = e->list) {
		execnode(e->target);
		c += 1;
	}

	callnode(n,c);
}



float samplenode(t_node *n) {
	int c = 0;
	for (t_edge *e = n->inlets; e != 0; e = e->list) {
		execnode(e->target);
		c += 1;
	}

	d_putint(c);
	n->pclass->method(n,DSP);
	return popf();
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



