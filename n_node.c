
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
		/* otherwise this is a trigger node and we should not mess with it because
		it could be on a different thread! */
		if (n->pclass->numoutlets != 0) {
			execnode(n);
		}
		return;
	}

	for (int i=0; i<arrlen(n->outlets); i+=1) {
		notify(n->outlets[i].target);
	}
}


t_box nodebox(t_node *n) {
	t_box b = n->box;
	/* replace with flag - todo */
	if (seldragnode == n) {
		int xdelta = 32 * ((lgi.Input.Mice.xcursor - dragnodexclick) / 32);
		int ydelta = 32 * ((lgi.Input.Mice.ycursor - dragnodeyclick) / 32);
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
	t_edge outlet;
	outlet.target = inletnode;
	outlet.inletslot = inletslot;
	outlet.outletslot = outletslot;
	*arradd(outletnode->outlets,1) = outlet;

	/* inlet, connects inletnode to outlet node */
	t_edge inlet;
	inlet.target = outletnode;
	inlet.inletslot = inletslot;
	inlet.outletslot = outletslot;
	*arradd(inletnode->inlets,1) = inlet;

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



