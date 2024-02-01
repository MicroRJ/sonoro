
/* default implementations */

void drawinletedge(t_node *source, t_edge inlet) {

	t_box s = nodebox(source);
	t_box i = getinletbox(s,inlet.inletslot);

	t_box t = nodebox(inlet.target);
	t_box o = getoutletbox(t,inlet.outletslot);

	drawline(i.x+i.zx*.5f, s.y+s.zy,o.x+o.zx*.5f,t.y, 4, UI_COLOR_FOREGROUND);
}


int basemethod(t_node *n, int k, int x, int y) {
	switch (k) {
		/* On 'CALL' we're expected to return the number of results
		we've outputted, by default we assume this node acts as a bypass
		and so we return the number of arguments. */
		case CALL: {
			_log("call %s-%i, %i $%i",n->pclass->name,n->id,x,istack);
			return x;
		} break;

#if 0
		case FEED: {
		} break;
		/* the exec method is used to trigger execution of the whole branch,
		 we first call the top-level nodes recursively and then us. */
		case EXEC: {
			int c = 0;
			int numinlets = arrlen(n->inlets);
			/* execute the inlets right to left, this will order the stack properly
			for function arguments, if the output of the right-most inlet is pushed
			first, then later when the user pops a value it will get the left-most
			one. */
			for (int i=numinlets-1; i>=0; i-=1) {
				t_node *t = n->inlets[i].target;
				/*
					Continue calling exec node before, which will drive the executing
				flow in an downwards manner, nodes at the very top get executed first. */
				int r = execnode(t,x,y);
				c += r;
				continue;
				///*
				//	Check how many outlets this node have, the node should have outputted
				//a number of values fit for all its outlets.
				// 	To keep the execution order the same (right to left) the stack order is
			 	//the opposite as before, in this case the value for the left most outlet is
			 	//pushed first, therefore the value for the right most outlet is last.. */
				//int numoutlets = arrlen(t->outlets);
				///* The first outlet is the one were the execution flow (EXEC) goes through,
				//so we iterate over all the other right most outlets in the proper execution
				//order.*/
				//for (int j=numoutlets-1; j>=1; j-=1) {
				//	t_node *o = t->outlets[j].target;
				//	/* Now because we're going backwards, 'call' gets issued first before feed,
				//	which feeds down the execution flow */
				//	r = o->pclass->method(o,CALL);
				//
				//	o->pclass->method(o,FEED);
				//}
			}
			_log("exec %s",n->pclass->name);

			// for (t_edge *e = n->inlets; e != 0; e = e->list) {
			// 	c += execnode(e->target);
			// }

			int r = invoke(n,CALL,c,0);
			/* check whether this node yielded something when there are no
			outlets, in which case, cleanup the stack */
			if (r != 0 && n->outlets == NULL && n->pclass->numoutlets != 0) {
				r = 0;
				for (int i=0;i<n->pclass->numoutlets;i+=1) {
					pop();
				}
			}
			return r;
		} break;
	#endif
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
			lgi_Color color = UI_COLOR_FOREGROUND;
			drawboxoutline(b,4.f,color);

			for (int i=0; i<n->pclass->numinlets; i+=1) {
				drawbox(getinletbox(b,i),UI_COLOR_FOREGROUND);
			}
			for (int i=0; i<n->pclass->numoutlets; i+=1) {
				drawbox(getoutletbox(b,i),UI_COLOR_FOREGROUND);
			}

			for (int j=0; j<arrlen(n->inlets); j+=1) {
				drawinletedge(n,n->inlets[j]);
			}
			if (n->label != 0) {
				drawtext(b,n->label);
			}
			return seldragnode == n;
		} break;
	}

	return 0;
}

void exportedge(t_node *n, char const *type, t_edge *e, t_exporter *i) {
	ini_writeheader(i,type);
	int source = n->id;
	int target = e->target->id;
	d_putint(source); ini_writefield(i,"source");
	d_putint(target); ini_writefield(i,"target");
	d_putint(e->outletslot); ini_writefield(i,"outletslot");
	d_putint(e->inletslot); ini_writefield(i,"inletslot");
}

void exportinlets(t_node *n, t_exporter *e) {
	for (int i=0; i<arrlen(n->inlets); i+=1) {
		exportedge(n,"inlet",&n->inlets[i],e);
	}
}

t_edge *importinlet(t_importer *i) {
	int source = 0,target = 0,outletslot = 0, inletslot = 0;
	char b[MAX_NAME];
	while (ini_nextfield(i,b)) {
		if(!strcmp(b,"source")) source = d_popint(); else
		if(!strcmp(b,"target")) target = d_popint(); else
		if(!strcmp(b,"outletslot")) outletslot = d_popint(); else
		if(!strcmp(b,"inletslot")) inletslot = d_popint(); else
		/* we break here to allow for subclasses
		 to import their properties */
		break;
	}
	n_addinlet(drawlist[source],inletslot,drawlist[target],outletslot);
}

t_node *importbasenode(t_class *c, t_importer *i) {
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

void exportbasenode(t_node *n, t_exporter *e) {
	ini_writeheader(e,n->pclass->name);
	d_putint(n->id); ini_writefield(e,"id");
	putx(n->label); ini_writefield(e,"label");
	d_putfloat(n->box.x); ini_writefield(e,"b.x");
	d_putfloat(n->box.y); ini_writefield(e,"b.y");
	d_putfloat(n->box.zx); ini_writefield(e,"b.zx");
	d_putfloat(n->box.zy); ini_writefield(e,"b.zy");
}

