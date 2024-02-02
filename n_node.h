
/* DAG: Directed Acyclic Graph:
			This programming language's data flow is modeled
		with a DAG.
			Each node represents some object that can be interfaced
		with bia a function.
			There are no intermediate variables so creating basic
		mechanisms can be challenging at first.

		Suppose you wanted to create an adder contraption such as:

				=> (A (+ A B))


		How would you model this using a graph without introducing
		circular dependencies?

		'A' depends on the output of '+' and '+' depends on the
		output of 'A'. Without the notion of temporary variables
		and imperative instructions this becomes impossible.

		But there are additions that make a mechanism like this
		possible here:

				- We offer 2 classes of directed edges which allow
			for complete control over the traversal algorithm,
			there are also other nodes that can do this and
			do so dynamically.
				- Nodes are both objects and functions, meaning that
			some nodes can hold data across pulses.
				In this case, we have a number node, which has one
			outlet and inlet and can be invoked even with no inputs in
			order to read the current number it holds.
			So in this way it acts as an object and or as a function.
				- The execution model is flexible.


			Inlet edges connect the input sockets of
		a node to the output sockets of another.
			The execution flow is given by the following graph
		traversal algorithm:


		Find a root node, traverse the tree starting at the root node
		in a depth first fashion, invoking each subsequent top-most node.
		Nodes at the very top (the leaves) get evaluated first, they will
		push values onto the stack, their predecessors get evaluated next,
		consuming those values from the stack, this process is repeated
		until the root node is finally evaluated.

		There are two other ways in which control flow branches out:

		If there are two types of possible branching:
		A node may have multiple inlets, in which case all inlets must
		be evaluated first before evaluating the node itself (as explained above).
		These are evaluated right to left, which will cause the stack
		to be conviently ordered.

		C([i,j]A(x,y))

		First, a root node is found, a root node is determinethis can be done in several
		ways, and every node has the potential of being the source
		so long it is the very root of a tree. However, there are
		specific types of nodes which are automatically found by
		the program and executed accordingly. These nodes are explict
		sources and have no output sockets. This means other nodes
		within the execution tree cannot form cyclical connections with
		them.


*/

/* A node system that allows me to create nodes using GUI,
	allows me to adjust their properties and connect them
	however I want */
typedef struct t_edge t_edge;
typedef struct t_class t_class;
typedef struct t_node t_node;

typedef struct t_unloader t_unloader;
typedef struct t_loader t_loader;

void ini_writeheader(t_unloader *, char const *);
void ini_writefield(t_unloader *, char const *);
int ini_nextheader(t_loader *, char *b);
int ini_nextfield(t_loader *, char *b);

enum {
	/* Use the 'CALL' method to execute only this node,
	method expects the number of arguments that are on
	the stack in implicit argument 'x'. */
	CALL,
	DRAW,
};

/* Depending on the method the result may take on different meanings,
 k means denotes the instruction type, x and y the implicit arguments. */
typedef int (t_method) (t_node *n, int k, int x, int y);
typedef t_node *(t_new) (t_class *c);
typedef void (t_unload) (t_node *n, t_unloader *);
typedef t_node *(t_load) (t_class *c, t_loader *);

typedef struct t_class {
	char name[MAX_NAME];
	t_new *constructor;
	t_method *method;
	t_unload *unload;
	t_load *load;
	/* remove from here! */
	int typesize;
	/* todo: this is node specific */
	int numinlets;
	int numoutlets;
} t_class;

enum {
	INVALID_WIRE,
	BLACK_WIRE,
	BLUE_WIRE,
	RED_WIRE,
};

typedef struct t_edge {
	char type;
	t_node *target;
	int outletslot,inletslot;
} t_edge;

typedef struct t_node {
	t_class *pclass;
	int id;
	char const *label;
	t_box box;

	int exectested;
	// int numoutputs;

	int numinlets;
	int numoutlets;
	/* these arrays contain the inlet and outlet sockets, index into
	them in the same order as the sockets are laid out, left-most is 0 */
	t_edge *inlets;
	t_edge *outlets;
} t_node;

int invoke(t_node *n, int k, int x, int y);
int drawnode(t_node *n);
int evalnode(t_node *n);
int callnode(t_node *n, int x, int y);

/* node exporter/importer, exports only the base properties */
t_node *loadbasenode(t_class *c, t_loader *i);
void unloadbasenode(t_node *n, t_unloader *e);
// /* todo: make this basenodemethod instead and do the default behavior for nodes there */
// void drawbasenode(t_node *n);

int basemethod(t_node *n, int k, int x, int y);
t_node *basenew(t_class *c, char const *label, t_box box);

t_node *loadnode(t_class *c, t_loader *i);
void unloadnode(t_node *n, t_unloader *e);



void notify(t_node *n);
void n_addoutlet(t_node *outletnode, int outletslot, t_node *inletnode, int inletslot, int type);
void connect(t_node *inletnode, int inletslot, t_node *outletnode, int outletslot, int type);

float wirethickness(int type) {
	int thickness[] = { 1, 4, 2, 2 };
	return thickness[type];
}

lgi_Color wirecolor(int type) {
	lgi_Color color[] = { lgi_YELLOW, lgi_BLACK, lgi_BLUE, lgi_RED  };
	return color[type];
}

/* builtin nodes */

/* can we implement some sort of caching or memoization? */


typedef struct t_toggle {
	t_node node;
	int toggled;
} t_toggle;
t_node *toggleconstructor(t_class *);
int togglemethod(t_node *n, int k, int x, int y);

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
void sliderexportmethod(t_node *n, t_unloader *e);
t_node *sliderimportmethod(t_class *c, t_loader *i);

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
