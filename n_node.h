
/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */
typedef struct t_edge t_edge;
typedef struct t_class t_class;
typedef struct t_node t_node;
typedef struct t_exporter t_exporter;
typedef struct t_importer t_importer;

enum {
	/* use this method to call or invoke a node, the arguments should be on the stack
	along with the number of arguments. The method returns the number of results and
	the results themselves are on the stack. */
	/* todo: this can be removed */
	CALL,
	/* similar to call, but it does not expect any arguments on the stack, instead
	it receives the arguments from its inlets by means of first issuing an exec
	message on its inlets before finally issuing a call on itself, this has the
	effect of evaluating the whole branch. */
	EXEC,

	FEED,
	TICK,
	DRAW,
};

/* Depending on the method the result may take on different meanings,
 k means denotes the instruction type, x and y the implicit arguments. */
typedef int (t_method) (t_node *n, int k, int x, int y);

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
	t_constructor *constructor;
	t_method *method;
	t_exportmethod *export_;
	t_importmethod *import_;
	/* todo: this is node specific */
	int numinlets;
	int numoutlets;
} t_class;

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

int invoke(t_node *n, int k, int x, int y);
int drawnode(t_node *n);
int execnode(t_node *n, int x, int y);
int callnode(t_node *n, int x, int y);

/* node exporter/importer, exports only the base properties */
t_node *importbasenode(t_class *c, t_importer *i);
void exportbasenode(t_node *n, t_exporter *e);
// /* todo: make this basenodemethod instead and do the default behavior for nodes there */
// void drawbasenode(t_node *n);

int basemethod(t_node *n, int k, int x, int y);
t_node *baseconstructor(t_class *c, char const *label, t_box box);

t_node *importnode(t_class *c, t_importer *i);
void exportnode(t_node *n, t_exporter *e);



void notify(t_node *n);
void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot);
void n_addinlet(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot);

/* builtin nodes */

/* can we implement some sort of caching or memoization? */


typedef struct t_detector {
	t_node node;
	int detectord;
} t_detector;
t_node *detectorconstructor(t_class *);
int detectormethod(t_node *n, int k, int x, int y);

typedef struct t_toggle {
	t_node node;
	int toggled;
} t_toggle;
t_node *toggleconstructor(t_class *);
int togglemethod(t_node *n, int k, int x, int y);



typedef struct t_timer {
	t_node node;
	double time;
	int tick;
} t_timer;
t_node *timerconstructor(t_class *);
int timermethod(t_node *n, int k, int x, int y);

typedef struct t_osc {
	t_node node;
	float hz;
	double time;
} t_osc;
t_node *oscconstructor(t_class *);
int oscmethod(t_node *n, int k, int x, int y);

typedef struct t_slider {
	t_node node;
	float min,max,val;
} t_slider;
t_node *sliderconstructor(t_class *);
int slidermethod(t_node *n, int k, int x, int y);
void sliderexportmethod(t_node *n, t_exporter *e);
t_node *sliderimportmethod(t_class *c, t_importer *i);

typedef struct t_graph {
	t_node node;
	float *buffer;
	int length;
	int cursor;
} t_graph;
t_node *graphconstructor(t_class *);
int graphmethod(t_node *n, int k, int x, int y);

typedef struct t_tick {
	t_node node;
	int enabled;
} t_tick;
t_node *tickconstructor(t_class *);
int tickmethod(t_node *n, int k, int x, int y);


typedef struct t_dac {
	t_node node;
	int enabled;
} t_dac;
t_node *dacconstructor(t_class *);
int dacmethod(t_node *n, int k, int x, int y);

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
int enginemethod(t_node *n, int k, int x, int y);

t_class **module;
