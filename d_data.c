#define MAX_NAME 32

enum {
	VERROR,
	VSTRING,
	VFLOAT,
	VINT,
};

char const *typetostring(int k) {
	char const *table[] = { "error", "obj", "num", "int" };
	return table[k];
}

typedef struct t_string {
	int length;
	char contents[1];
} t_string;

typedef struct t_value {
	char k;
	union {
		float f;
		int i;
		t_string *s;
	};
} t_value;

int getk();

int d_popmask();

float d_popfloat();
int d_popint();
char *popx();
t_value pop();
void rt_pushvalue(t_value);
t_value *d_putk(int k);

void d_putmask(int x);

void d_putfloat(float x);
void d_putint(int x);
void putx(char const *x);

void d_dup();

t_string *newstr(char const *x);


//
__declspec(thread) t_value stack[0x100];
__declspec(thread) int istack;

t_string *newstr(char const *contents) {
	if (contents == 0) {
		return 0;
	}
	int length = strlen(contents);
	t_string *s = calloc(1,sizeof(t_string)+length+1);
	memcpy(s->contents,contents,length+1);
	return s;
}

int getk() {
	return stack[istack-1].k;
}

void rt_pushvalue(t_value v) {
	if (istack >= _countof(stack)) {
		istack = _countof(stack) - 1;
		_err("stack overflow");
	}
	_log("put $%i, %i",istack,v.k);
	stack[istack ++] = v;
}

t_value *d_putk(int k) {
	if (istack >= _countof(stack)) {
		istack = _countof(stack) - 1;
		_err("stack overflow");
	}
	_log("put: $%i, %i",istack,k);
	t_value *v = stack + istack ++;
	v->k = k;
	return v;
}

void d_dup() {
	if (istack >= _countof(stack)) {
		istack = _countof(stack) - 1;
		_err("stack overflow");
	}
	stack[istack] = stack[istack ++ - 1];
}

void d_putfloat(float f) {
	d_putk(VFLOAT)->f = f;
}
void d_putint(int i) {
	d_putk(VINT)->i = i;
}
void putx(char const *s) {
	d_putk(VSTRING)->s = newstr(s);
}
t_value pop() {
	if (istack <= 0) {
		__debugbreak();
		_log("stack underflow");
		return (t_value){0};
	}
	_log("pop: $%i",istack-1);
	return stack[-- istack];
}
char *popx() {
	lgi_ASSERT(getk() == VSTRING);
	t_string *s = pop().s;
	if (s == 0) return 0;
	return s->contents;
}
float d_popfloat() {
	if (getk() == VFLOAT) return pop().f;
	if (getk() == VINT) return (float) pop().i;
	_war("invalid data type");
	return 0;
}
int d_popint() {
	if (getk() == VINT) return pop().i;
	if (getk() == VFLOAT) return (int) pop().f;
	_war("invalid data type");
	return 0;
}
