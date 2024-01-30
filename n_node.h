
/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */
typedef struct t_node t_node;
typedef struct t_edge t_edge;

typedef struct t_edge {
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

void loadbuiltins();

/* node exporter/importer, exports only the base properties */
t_node *importbasenode(t_class *c, t_importer *i);
void exportbasenode(t_node *n, t_exporter *e);
// /* todo: make this basenodemethod instead and do the default behavior for nodes there */
// void drawbasenode(t_node *n);

int basemethod(t_node *n, int k);
t_node *baseconstructor(t_class *c, char const *label, t_box box);

t_node *importnode(t_class *c, t_importer *i);
void exportnode(t_node *n, t_exporter *e);

void drawnode(t_node *n);
int execnode(t_node *n);

void notify(t_node *n);
void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot);
void n_addinlet(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot);

/* builtin nodes */

/* can we implement some sort of caching or memoization? */
typedef struct t_num {
	t_node node;
	float num;
} t_num;
t_node *numconstructor(t_class *);
int nummethod(t_node *n, int k);

typedef struct t_toggle {
	t_node node;
	int toggled;
} t_toggle;
t_node *toggleconstructor(t_class *);
int togglemethod(t_node *n, int k);

typedef struct t_div {
	t_node node;
} t_div;
t_node *divconstructor(t_class *);
int divmethod(t_node *n, int k);

typedef struct t_add {
	t_node node;
} t_add;
t_node *addconstructor(t_class *);
int addmethod(t_node *n, int k);

typedef struct t_min {
	t_node node;
} t_min;
t_node *minconstructor(t_class *);
int minmethod(t_node *n, int k);

typedef struct t_osc {
	t_node node;
	float hz;
	double time;
} t_osc;
t_node *oscconstructor(t_class *);
int oscmethod(t_node *n, int k);

typedef struct t_slider {
	t_node node;
	float min,max,val;
} t_slider;
t_node *sliderconstructor(t_class *);
int slidermethod(t_node *n, int k);
void sliderexportmethod(t_node *n, t_exporter *e);
t_node *sliderimportmethod(t_class *c, t_importer *i);

typedef struct t_graph {
	t_node node;
	float *buffer;
	int length;
	int cursor;
} t_graph;
t_node *graphconstructor(t_class *);
int graphmethod(t_node *n, int k);

typedef struct t_tick {
	t_node node;
	int enabled;
} t_tick;
t_node *tickconstructor(t_class *);
int tickmethod(t_node *n, int k);


typedef struct t_dac {
	t_node node;
	int enabled;
} t_dac;
t_node *dacconstructor(t_class *);
int dacmethod(t_node *n, int k);

/* remove from here you big C */
typedef struct t_engine {
	t_node node;
	float  revspermin;
	double crankshaftrotation;
	float  crankarmradius;
	float  pistonwidth;
	float  connectingrodlength;
	float  connectingrodthickness;
	/* the length from the center of the crankshaft to the center of the rod-journal */
} t_engine;
t_node *engineconstructor(t_class *);
int enginemethod(t_node *n, int k);

t_class **module;
