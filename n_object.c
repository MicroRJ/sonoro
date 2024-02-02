
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

t_class *newclass(char const *name, int typesize, int numinlets, int numoutlets, t_new *constructor, t_method *method, t_unload *unload, t_load *load) {
	t_class *c = calloc(1,sizeof(t_class));
	strcpy(c->name,name);
	c->typesize = typesize;
	c->numinlets = numinlets;
	c->numoutlets = numoutlets;
	c->constructor = constructor;
	c->method = method;
	c->unload = unload;
	c->load = load;

	*arradd(module,1) = c;
	return c;
}

