
t_node *seldragnode;
t_node *selinletnode;
int selinletslot;
int dragnodexclick;
int dragnodeyclick;

/* remove from here */
t_node **drawlist;

/* the last node id, ensure that when importing nodes, the id is restored, or made
 bigger than the largest id imported */
int lastnodeid;

t_node *baseconstructor(t_class *pclass, char const *label, t_box box) {
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

/* invoke */
int invoke(t_node *n, int k, int x, int y) {
	return n->pclass->method(n,k,x,y);
}

int drawnode(t_node *n) {
	return invoke(n,DRAW,0,0);
}

int callnode(t_node *n, int x, int y) {
	return invoke(n,CALL,x,y);
}
//
// tick(num(),timer()) != tick(num(timer()))
//

/* drives the execution downwards */
int feedoverride(t_node *n, int x, int y) {
	int numresults = callnode(n,x,y);
	int numoutlets = arrlen(n->outlets);
	for (int i=numoutlets-1; i>=0; i-=1) {
		numresults = feedoverride(n->outlets[i].target,numresults,0);
	}
	return numresults;
}
int execoverride(t_node *n, int x, int y) {
	int numinlets = arrlen(n->inlets);
	int numvalues = 0;
	for (int i=numinlets-1; i>=0; i-=1) {
		t_node *t = n->inlets[i].target;
		numvalues += execoverride(t,x,y);
		int numoutlets = arrlen(t->outlets);
		for (int j=numoutlets-1;j>=1;j-=1) {
			numvalues = feedoverride(t->outlets[j].target,numvalues,0);
		}
	}
	return callnode(n,numvalues,0);
}

int execnode(t_node *n, int x, int y) {
	return execoverride(n,x,y);
	// return invoke(n,EXEC,x,y);
}

void notify(t_node *n) {
	if (n->outlets == 0 && n->inlets != 0) {
		/* otherwise this is a trigger node and we should not mess with it because
		it could be on a different thread! */
		if (n->pclass->numoutlets != 0) {
			execnode(n,0,0);
		}
		return;
	}

	for (int i=arrlen(n->outlets)-1; i>=0; i+=1) {
		notify(n->outlets[i].target);
	}
}


t_box nodebox(t_node *n) {
	t_box b = n->box;
	/* replace with flag - todo */
	if (seldragnode == n) {
		int xdelta = 32 * ((getxcursor() - dragnodexclick) / 32);
		int ydelta = 32 * ((getycursor() - dragnodeyclick) / 32);
		b.x += xdelta;
		b.y += ydelta;
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



t_box getinletbox(t_box b, int i) {
	int h = 8;
	return bbox(b.x+h+i*32,b.y+b.zy+4,h*2,8);
}

t_box getoutletbox(t_box b, int i) {
	int h = 8;

	return bbox(b.x+h+i*32,b.y-8-4,h*2,8);
}



