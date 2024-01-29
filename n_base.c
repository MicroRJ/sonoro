
/* default implementations */

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
	for (t_edge *j = n->inlets; j != 0; j = j->list) {
		exportedge(n,"inlet",j,e);
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
	t_node *n = ininode(c);

	char b[MAX_NAME];
	while (ini_nextfield(i,b)) {
		if(!strcmp(b,"id")) n->id = d_popint(); else
		if(!strcmp(b,"label")) n->label = popx(); else
		if(!strcmp(b,"b.x0")) n->box.x0 = popf(); else
		if(!strcmp(b,"b.y0")) n->box.y0 = popf(); else
		if(!strcmp(b,"b.x1")) n->box.x1 = popf(); else
		if(!strcmp(b,"b.y1")) n->box.y1 = popf(); else
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
	d_putfloat(n->box.x0); ini_writefield(e,"b.x0");
	d_putfloat(n->box.y0); ini_writefield(e,"b.y0");
	d_putfloat(n->box.x1); ini_writefield(e,"b.x1");
	d_putfloat(n->box.y1); ini_writefield(e,"b.y1");
}

int dragnode_(t_node *n) {
	lui_Box b = nodebox(n);

	if (lgi_testCtrlKey()) {
		/* make it a little bit easier to click */
		if (clicked(lui_Box_enlarge(b,32.f,32.f))) {
			for (int i=0; i<n->pclass->numinlets; i+=1) {
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

void drawbasenode(t_node *n) {
	lui_Box b = nodebox(n);

	lgi_Color color = UI_COLOR_FOREGROUND;
	drawboxoutline(b,4.f,color);

	for (int i=0; i<n->pclass->numinlets; i+=1) {
		lui__drawBox(getinletbox(b,i),UI_COLOR_FOREGROUND);
	}
	for (int i=0; i<n->pclass->numoutlets; i+=1) {
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
