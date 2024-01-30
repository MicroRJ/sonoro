typedef struct t_class t_class;
/* todo: an object should be its own separate thing */
typedef struct t_node t_node;
typedef t_node t_object;

typedef struct t_exporter t_exporter;
typedef struct t_importer t_importer;

typedef t_object *(t_constructor) (t_class *c);

enum {
	/* use this method to call or invoke a node, the arguments should be on the stack
	along with the number of arguments. The method returns the number of results and
	the results themselves are on the stack. */
	CALL,
	/* similar to call, but it does not expect any arguments on the stack, instead
	it receives the arguments from its inlets by means of first issuing an exec
	message on its inlets before finally issuing a call on itself, this has the
	effect of evaluating the whole branch. */
	EXEC,
	TICK,
	DRAW,
};

/* depending on the method the result may take on different meanings*/
typedef int (t_method) (t_object *n, int k);

typedef void (t_exportmethod) (t_object *n, t_exporter *);
typedef t_object *(t_importmethod) (t_class *c, t_importer *);

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