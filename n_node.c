
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

/* execution order testing and visualization */
int test_fallexecorder(t_node *n, int x, int y) {
	if (n->exectested) {
		for (int i=n->pclass->numoutlets-1; i>=0; i-=1) {
			t_edge outlet = n->outlets[i];
			if (outlet.target == 0) continue;
			if (outlet.type == RED_WIRE) continue;
			if(test_fallexecorder(outlet.target,n->numoutputs,0)) {
				return 1;
			}
		}
		return 0;
	}
	n->numoutputs = callnode(n,x,y);
	n->exectested = 1;
	return 1;
}

int test_execorder(t_node *n) {
	if (n->exectested) {
		return 0;
	}
	for (int i=n->pclass->numinlets-1; i>=0; i-=1) {
		t_edge inlet = n->inlets[i];
		if (inlet.target == 0) continue;
		if (inlet.type == BLUE_WIRE) continue;
		if (test_execorder(inlet.target)) {
			return 1;
		}
		for (int j=inlet.target->pclass->numoutlets-1;j>=0;j-=1) {
			if (j == inlet.outletslot) continue;
			t_edge outlet = inlet.target->outlets[j];
			if (outlet.target == 0) continue;
			if (outlet.type == RED_WIRE) continue;
			if (test_fallexecorder(outlet.target,inlet.target->numoutputs,0)) {
				return 1;
			}
		}
	}
	n->exectested = 1;
	n->numoutputs = callnode(n,0,0);
	return 1;
}


t_node *basenew(t_class *pclass, char const *label, t_box box) {
	t_node *n = calloc(1,pclass->typesize);
	n->id = lastnodeid ++;
	n->pclass = pclass;
	n->label = label;
	n->box = box;
	n->outlets = calloc(pclass->numoutlets,sizeof(t_edge));
	n->inlets = calloc(pclass->numinlets,sizeof(t_edge));
	return n;
}

void unloadnode(t_node *n, t_unloader *e) {
	if (n->pclass->unload != 0) {
		n->pclass->unload(n,e);
	} else {
		unloadbasenode(n,e);
	}
}

t_node *loadnode(t_class *c, t_loader *i) {
	if (c->load != 0) {
		return c->load(c,i);
	} else {
		return loadbasenode(c,i);
	}
}

/* invoke */
int invoke(t_node *n, int k, int x, int y) {
	return n->pclass->method(n,k,x,y);
}

int drawnode(t_node *n) {
	return invoke(n,DRAW,0,0);
}

int callnode(t_node *n, int x, int y) {
	if (x > istack) {
		_err("invalid call");
		__debugbreak();
	}
	int result = invoke(n,CALL,x,y);
	if (result > istack) {
		_err("invalid result");
		__debugbreak();
	}
	return result;
}


/* drives the execution downwards */
int feednode(t_node *n, int x, int y) {
	int numresults = callnode(n,x,y);
	_log("feed '%s': %i => %i",n->pclass->name,x,numresults);

	for (int i=n->pclass->numoutlets-1; i>=0; i-=1) {
		t_edge outlet = n->outlets[i];
		/* we can only go down red blue and black wires */
		if (outlet.type == RED_WIRE) continue;
		/* ensure this outlet is enabled */
		if (outlet.target == 0) continue;

		numresults = feednode(outlet.target,numresults,0);
	}
	return numresults;
}

/* drives the execution flow upwards */
int execnode(t_node *n, int x, int y) {
	lgi_ASSERT(x == 0); /* Since exec node is a depth
	first traversal, the leaf nodes are to be terminal nodes
	and thus take no inputs, or at least don't require inputs.. */
	int numresults = 0;
	for (int i=n->pclass->numinlets-1; i>=0; i-=1) {
		t_edge inlet = n->inlets[i];
		/* is this inlet even enabled */
		if (inlet.target == 0) continue;
		/* blue wires disallow the flow upwards */
		if (inlet.type == BLUE_WIRE) continue;
		lgi_ASSERT(inlet.inletslot == i);

		numresults += execnode(inlet.target,x,y);

		/* execute hanging branches, these are the outlets of
		the inlet which are not connected to us, and would not
		get executed otherwise, note that after we start executing
		hanging branches we only traverse the graph downwards. */
		for (int j=inlet.target->pclass->numoutlets-1;j>=0;j-=1) {
			t_edge outlet = inlet.target->outlets[j];
			/* ensure this outlet is enabled */
			if (outlet.target == 0) continue;
			/* the branch from where we came from is
			not a 'hanging-branch' and if we did execute then
			the graph would become cyclical */
			if (j == inlet.outletslot) continue;
			/* we can only go down red blue and black wires */
			if (outlet.type == RED_WIRE) continue;

			lgi_ASSERT(outlet.outletslot == j);

			numresults = feednode(outlet.target,numresults,0);
		}
	}
	_log("exec: (%s_%i)() => %i",n->pclass->name,n->id,numresults);
	return callnode(n,numresults,0);
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

	for (int i=n->pclass->numoutlets-1; i>=0; i-=1) {
		t_node *k = n->outlets[i].target;
		if (k == 0) continue; /* ensure outlet is enabled */
		notify(k);
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

void connect(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot, int type) {
	n_addoutlet(outletnode,outletslot,inletnode,inletslot,type);
}

void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot, int type) {
	if (outletslot >= outletnode->pclass->numoutlets) {
		_log("too many outlets");
	}
	if (inletslot >= inletnode->pclass->numinlets) {
		_log("too many inlets");
	}

	t_edge *outlet = outletnode->outlets + outletslot;
	if (outlet->target != 0) {
		_log("already connected");
		return;
	}

	t_edge *inlet = inletnode->inlets + inletslot;
	if (inlet->target != 0) {
		_log("already connected");
		return;
	}

	outlet->type = type;
	outlet->target = inletnode;
	outlet->inletslot = inletslot;
	outlet->outletslot = outletslot;
	outletnode->numoutlets += 1;

	inlet->type = type;
	inlet->target = outletnode;
	inlet->inletslot = inletslot;
	inlet->outletslot = outletslot;
	inletnode->numinlets += 1;

}


t_box getinletbox(t_box b, int i) {
	int h = 8;
	return bbox(b.x+h+i*32,b.y+b.zy+4,h*2,8);
}

t_box getoutletbox(t_box b, int i) {
	int h = 8;

	return bbox(b.x+h+i*32,b.y-8-4,h*2,8);
}

void drawinletedge(t_node *source, t_edge inlet) {

	t_box s = nodebox(source);
	t_box i = getinletbox(s,inlet.inletslot);

	t_box t = nodebox(inlet.target);
	t_box o = getoutletbox(t,inlet.outletslot);

	drawline(i.x+i.zx*.5f, s.y+s.zy,o.x+o.zx*.5f,t.y, wirethickness(inlet.type), wirecolor(inlet.type));
}

int basemethod(t_node *n, int k, int x, int y) {
	switch (k) {
		case CALL: {
			_log("$%i: (%s-%i %i)",istack,n->pclass->name,n->id,x);
			return x;
		} break;
		/* I think draw should get its own thing */
		case DRAW: {
			t_box b = nodebox(n);

			if (lgi_testCtrlKey()) {
				/* make it a little bit easier to click */
				if (clicked(growbox(b,32,32))) {
					for (int i=0; i<n->pclass->numinlets; i+=1) {
						if (clicked(growbox(getinletbox(b,i),8,8))) {
							selinletnode = n;
							selinletslot = i;
							return lgi_True;
						}
					}
					if (clicked(b)) {
						dragnodexclick = getxcursor();
						dragnodeyclick = getycursor();
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

			/* drawing */

			if (n->exectested) {
				drawbox(b,lgi_RED);
			}
			lgi_Color color = UI_COLOR_FOREGROUND;
			drawboxoutline(b,4.f,color);

			for (int i=0; i<n->pclass->numinlets; i+=1) {
				if (n->inlets[i].target == 0) continue;
				drawinletedge(n,n->inlets[i]);
			}

			for (int i=0; i<n->pclass->numinlets; i+=1) {
				drawbox(getinletbox(b,i),UI_COLOR_FOREGROUND);
			}
			for (int i=0; i<n->pclass->numoutlets; i+=1) {
				drawbox(getoutletbox(b,i),UI_COLOR_FOREGROUND);
			}
			if (n->label != 0) {
				drawtext(b,n->label);
			}
			return seldragnode == n;
		} break;
	}

	return 0;
}

t_edge *loadinlet(t_loader *i) {
	int type = 0, source = 0, target = 0, outletslot = 0, inletslot = 0;
	char b[MAX_NAME];
	while (ini_nextfield(i,b)) {
		if(!strcmp(b,"type")) type = d_popint(); else
		if(!strcmp(b,"source")) source = d_popint(); else
		if(!strcmp(b,"target")) target = d_popint(); else
		if(!strcmp(b,"outletslot")) outletslot = d_popint(); else
		if(!strcmp(b,"inletslot")) inletslot = d_popint(); else
		/* we break here to allow for subclasses
		 to import their properties */
		break;
	}
	connect(drawlist[source],inletslot,drawlist[target],outletslot,type);
}

void unloadinlet(t_node *n, t_edge e, t_unloader *i) {
	ini_writeheader(i,"inlet");
	int source = n->id;
	int target = e.target->id;
	d_putint(e.type); ini_writefield(i,"type");
	d_putint(source); ini_writefield(i,"source");
	d_putint(target); ini_writefield(i,"target");
	d_putint(e.outletslot); ini_writefield(i,"outletslot");
	d_putint(e.inletslot); ini_writefield(i,"inletslot");
}

void unloadinlets(t_node *n, t_unloader *e) {
	for (int i=0; i<n->pclass->numinlets; i+=1) {
		if (n->inlets[i].target != 0) {
			unloadinlet(n,n->inlets[i],e);
		}
	}
}

t_node *loadbasenode(t_class *c, t_loader *i) {
	t_node *n = newobj(c);

	float x0 = 0,y0 = 0,x1 = 0,y1 = 0;
	char b[MAX_NAME];
	while (ini_nextfield(i,b)) {
		if(!strcmp(b,"id")) n->id = d_popint(); else
		if(!strcmp(b,"label")) n->label = popx(); else

		if(!strcmp(b,"b.x0")) n->box.x = d_popfloat(); else
		if(!strcmp(b,"b.y0")) n->box.y = d_popfloat(); else
		if(!strcmp(b,"b.x1")) n->box.zx = n->box.x - d_popfloat(); else
		if(!strcmp(b,"b.y1")) n->box.zy = n->box.y - d_popfloat(); else

		if(!strcmp(b,"b.x")) n->box.x = d_popfloat(); else
		if(!strcmp(b,"b.y")) n->box.y = d_popfloat(); else
		if(!strcmp(b,"b.zx")) n->box.zx = d_popfloat(); else
		if(!strcmp(b,"b.zy")) n->box.zy = d_popfloat(); else
		/* we break here to allow for subclasses
		 to import their properties */
		break;
	}

	return n;
}

void unloadbasenode(t_node *n, t_unloader *e) {
	ini_writeheader(e,n->pclass->name);
	d_putint(n->id); ini_writefield(e,"id");
	putx(n->label); ini_writefield(e,"label");
	d_putfloat(n->box.x); ini_writefield(e,"b.x");
	d_putfloat(n->box.y); ini_writefield(e,"b.y");
	d_putfloat(n->box.zx); ini_writefield(e,"b.zx");
	d_putfloat(n->box.zy); ini_writefield(e,"b.zy");
}





