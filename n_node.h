
/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */

typedef lui_Box t_box;
typedef struct t_node t_node;
typedef struct t_edge t_edge;
typedef struct t_class t_class;

enum {
	CALL,
	DSP,
	DRAW,
};

typedef void (t_method) (t_node *n, int k);
typedef t_node *(t_constructor) (t_class *c);

typedef void (t_exportmethod) (t_node *n, t_exporter *);
typedef t_node *(t_importmethod) (t_class *c, t_importer *);

typedef struct t_field {
	char name[MAX_NAME];
	char k;
} t_field;

typedef struct t_class {
	char name[MAX_NAME];
	int typesize;
	int numinlets;
	int numoutlets;
	t_constructor *constructor;
	t_method *method;
	/* this is for when we have fields */
	// t_constructor *constructor;
	/* remove this when we have fields */
	t_exportmethod *export_;
	t_importmethod *import_;
} t_class;

typedef struct t_edge {
	t_edge *list;
	t_node *target;
	int outletslot,inletslot;
} t_edge;

typedef struct t_node {
	t_class *pclass;
	int id;
	char const *label;
	t_edge *outlets;
	t_edge *inlets;
	t_box box;
} t_node;

/*
typedef struct t_canvas {
	t_node node;
} t_canvas;
*/

void n_moduleinit();

/* node exporter/importer, exports only the base properties */
t_node *importbasenode(t_class *c, t_importer *i);
void exportbasenode(t_node *n, t_exporter *e);
/* todo: make this basenodemethod instead and do the default behavior for nodes there */
void drawbasenode(t_node *n);

t_node *importnode(t_class *c, t_importer *i);
void exportnode(t_node *n, t_exporter *e);
void drawnode(t_node *n);
void callnode(t_node *n, int c);
void notify(t_node *n);
void execnode(t_node *n);
t_node *newnode(t_class *c, char const *label, t_box box);
void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot);
void n_addinlet(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot);


/* builtin nodes */
typedef struct t_exporter t_exporter;
typedef struct t_importer t_importer;

typedef struct t_num {
	t_node node;
	float num;
} t_num;
t_node *numconstructor(t_class *);
void nummethod(t_node *n, int k);
typedef struct t_min {
	t_node node;
} t_min;
t_node *minconstructor(t_class *);
void minmethod(t_node *n, int k);
typedef struct t_osc {
	t_node node;
	float hz;
	double time;
} t_osc;
t_node *oscconstructor(t_class *);
void oscmethod(t_node *n, int k);
typedef struct t_slider {
	t_node node;
	float min,max,val;
} t_slider;
t_node *sliderconstructor(t_class *);
void slidermethod(t_node *n, int k);
void sliderexportmethod(t_node *n, t_exporter *e);
t_node *sliderimportmethod(t_class *c, t_importer *i);
typedef struct t_graph {
	t_node node;
	int samples;
} t_graph;
t_node *graphconstructor(t_class *);
void graphmethod(t_node *n, int k);

typedef struct t_dac {
	t_node node;
} t_dac;
t_node *dacconstructor(t_class *);
void dacmethod(t_node *n, int k);

t_class **module;
