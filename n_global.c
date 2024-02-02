
struct {
	char configfile[MAX_PATH];

	struct {
		int wireType;

	} Editor;

	struct {
		ma_context context;
		ma_result lastError;
		struct {
			ma_device handle;
			struct {
				ma_device_info * array;
				ma_uint32        count;
			} Playback;
			struct {
				ma_device_info * array;
				ma_uint32        count;
			} Capture;
		} Device;

		struct {
			float frequency;
			// TODO: Can We Track This In Frames?
			double time;
			float volume;
		} TestSignal;
	} Audio;
} lgi_Global App;

void loadbuiltins() {
	newclass( "num", sizeof(t_num), 1, 2, newnum, nummethod, 0, 0);
	newclass( "dac", sizeof(t_dac), 2, 0, dacconstructor, dacmethod, 0, 0);
	newclass( "graph", sizeof(t_graph), 1, 1, graphconstructor, graphmethod, 0, 0);

	newclass( "timer", sizeof(t_timer), 1, 2, timerconstructor, timermethod, 0, 0);

	newclass( "detector", sizeof(t_detector), 1, 3, detectorconstructor, detectormethod, 0, 0);
	newclass( "splitter", sizeof(t_splitter), 1, 2, splitterconstructor, splittermethod, 0, 0);

	newclass( "toggle", sizeof(t_toggle), 1, 1, toggleconstructor, togglemethod, 0, 0);
	newclass( "osc", sizeof(t_osc), 1, 1, oscconstructor, oscmethod, 0, 0);
	newclass( "tick", sizeof(t_tick), 2, 0, tickconstructor, tickmethod, 0, 0);
	newclass( "slider", sizeof(t_slider), 1, 1, sliderconstructor, slidermethod, sliderexportmethod, sliderimportmethod );
	newclass( "add", sizeof(t_add), 2, 1, newadd, addmethod, 0, 0);
	newclass( "div", sizeof(t_div), 2, 1, newdiv, divmethod, 0, 0);
	newclass( "min", sizeof(t_min), 2, 1, newmin, minmethod, 0, 0);
	newclass( "engine", sizeof(t_engine), 4, 1, engineconstructor, enginemethod, 0, 0);
}

void editor() {
	if (selinletnode != lgi_Null) {
		float xcursor = getxcursor();
		float ycursor = getycursor();
		t_box in = getinletbox(nodebox(selinletnode),selinletslot);

		drawline(in.x,in.y,xcursor,ycursor,2,lgi_RED);

		for (int i=0; i<arrlen(drawlist); i+=1) {
			t_node *t = drawlist[i];
			if (t == selinletnode) continue;
			for (int j=0; j<t->pclass->numoutlets; j+=1) {
				t_box h = getoutletbox(nodebox(t),j);
				if (inbox(h,xcursor,ycursor)) {
					/* We're drawing an inlet to an outlet */
					connect(selinletnode,selinletslot,t,j,App.Editor.wireType);

					lgi_logInfo("connected!");
					selinletnode = lgi_Null;
					selinletslot = 0;
					break;
				}
			}
		}
	}
}

/* remove this from here */
void addnode(t_node *n) {
	*arradd(drawlist,1) = n;
}

int loadconfig(char const *filename) {
	if (filename == 0) {
		filename = ".ini";
	}
	FILE *file = fopen(filename,"rb");
	if (file == NULL) {
		_log("failed to read file");
		return 0;
	}
	if(fseek(file,0,SEEK_END) != 0) {
		_log("failed to read file");
		fclose(file);
		return 0;
	}

	strcpy(App.configfile,filename);

	long length = ftell(file);
	if (length == 0) {
		fclose(file);
		return 1;
	}

	fseek(file,0,SEEK_SET);

	char *buffer = malloc(length+1);
	fread(buffer,1,length,file);
	buffer[length] = 0;

	t_loader i = { buffer };
	char b[0x100];
	while (ini_nextheader(&i,b)) {
		t_class *c = findclass(b);
		if (c != 0) {
			t_node *n = loadnode(c,&i);
			addnode(n);

			if (n->id >= lastnodeid) {
				lastnodeid = n->id + 1;
			}
		} else
		if (!strcmp("inlet",b)) {
			loadinlet(&i);
			/* todo: */
		} else {
			_log("class not found");
		}
	}

	free(buffer);
	fclose(file);
	return 1;
}

void saveconfig() {
	char const *filename = App.configfile;
	if (filename == 0) {
		filename = "$_tmp.ini";
	}
	FILE *file = fopen(App.configfile,"wb");
	t_unloader e = { file };
	for (int i=0; i<arrlen(drawlist); i+=1) {
		unloadnode(drawlist[i],&e);
	}
	for (int i=0; i<arrlen(drawlist); i+=1) {
		unloadinlets(drawlist[i],&e);
	}
	fclose(file);
}
