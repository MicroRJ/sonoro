
t_node *newobj(t_class *c) {
	return c->constructor(c);
}

t_class *findclass(char const *name) {
	for (int i = 0; i < arrlen(module); i += 1) {
		t_class *c = module[i];
		if (!strcmp(c->name,name)) {
			return c;
		}
	}
	return 0;
}

t_class *newclass(char const *name, int typesize, int numinlets, int numoutlets, t_constructor *constructor, t_method *method, t_exportmethod *mexport, t_importmethod *mimport) {
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

void loadbuiltins() {
	newclass( "num", sizeof(t_num), 1, 1, numconstructor, nummethod, 0, 0);
	newclass( "toggle", sizeof(t_toggle), 1, 1, toggleconstructor, togglemethod, 0, 0);
	newclass( "add", sizeof(t_add), 2, 1, addconstructor, addmethod, 0, 0);
	newclass( "div", sizeof(t_div), 2, 1, divconstructor, divmethod, 0, 0);
	newclass( "min", sizeof(t_min), 2, 1, minconstructor, minmethod, 0, 0);
	newclass( "osc", sizeof(t_osc), 1, 1, oscconstructor, oscmethod, 0, 0);
	newclass( "slider", sizeof(t_slider), 1, 1, sliderconstructor, slidermethod, sliderexportmethod, sliderimportmethod );
	newclass( "graph", sizeof(t_graph), 1, 1, graphconstructor, graphmethod, 0, 0);
	newclass( "dac", sizeof(t_dac), 2, 0, dacconstructor, dacmethod, 0, 0);
	newclass( "tick", sizeof(t_tick), 2, 0, tickconstructor, tickmethod, 0, 0);
	newclass( "engine", sizeof(t_engine), 4, 1, engineconstructor, enginemethod, 0, 0);
}
