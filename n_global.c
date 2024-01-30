

/* remove this from here */
void addnode(t_node *n) {
	*arradd(drawlist,1) = n;
}

int load() {
	FILE *file = fopen(".ini","rb");
	if (file == NULL) {
		_log("failed to read file");
		return 0;
	}
	if(fseek(file,0,SEEK_END) != 0) {
		_log("failed to read file");
		fclose(file);
		return 0;
	}
	long length = ftell(file);
	if (length == 0) {
		_log("file is empty");
		fclose(file);
		return 0;
	}
	fseek(file,0,SEEK_SET);
	char *buffer = malloc(length+1);
	fread(buffer,1,length,file);
	buffer[length] = 0;

	t_importer i = { buffer };
	char b[0x100];
	while (ini_nextheader(&i,b)) {
		t_class *c = findclass(b);
		if (c != 0) {
			t_node *n = importnode(c,&i);
			addnode(n);

			if (n->id >= lastnodeid) {
				lastnodeid = n->id + 1;
			}
		} else
		if (!strcmp("inlet",b)) {
			importinlet(&i);
			/* todo: */
		} else {
			_log("class not found");
		}
	}

	free(buffer);
	fclose(file);
	return 1;
}

void save() {
	FILE *file = fopen(".ini","wb");
	t_exporter e = { file };
	for (int i=0; i<arrlen(drawlist); i+=1) {
		exportnode(drawlist[i],&e);
	}
	for (int i=0; i<arrlen(drawlist); i+=1) {
		exportinlets(drawlist[i],&e);
	}
	fclose(file);
}

void exec() {
	for (int i=0; i<arrlen(drawlist); i+=1) {
		t_node *n = drawlist[i];
		/* find end of execution branch */
		if (n->inlets != 0 && n->outlets == 0) {
			execnode(n);
		}
	}
}