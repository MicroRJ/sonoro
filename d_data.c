#define MAX_NAME 32

enum {
	VERROR,
	VFLOAT,
	VINT,
	VSTRING,
};

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
void d_put();

void d_putmask(int x);

void d_putfloat(float x);
void d_putint(int x);
void putx(char const *x);

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
void d_putfloat(float f) {
	if (istack >= _countof(stack)) {
		__debugbreak();
		_log("stack overflow");
		return;
	}
	stack[istack].k = VFLOAT;
	stack[istack].f = f;
	istack += 1;
}
void d_putint(int i) {
	if (istack >= _countof(stack)) {
		__debugbreak();
		_log("stack overflow");
		return;
	}
	stack[istack].k = VINT;
	stack[istack].i = i;
	istack += 1;
}
void d_put(t_value v) {
	if (istack >= _countof(stack)) {
		__debugbreak();
		_log("stack overflow");
		return;
	}
	stack[istack ++] = v;
}
void putx(char const *s) {
	if (istack >= _countof(stack)) {
		__debugbreak();
		_log("stack overflow");
		return;
	}
	stack[istack].k = VSTRING;
	stack[istack].s = newstr(s);
	istack += 1;
}
t_value pop() {
	if (istack <= 0) {
		__debugbreak();
		_log("stack underflow");
		return (t_value){0};
	}
	return stack[-- istack];
}


char *popx() {
	if (istack <= 0) {
		__debugbreak();
		_log("stack underflow");
		return 0;
	}
	if (stack[istack-1].k != VSTRING) {
		_log("invalid data type");
	}
	t_string *s = stack[-- istack].s;
	if (s == 0) {
		return 0;
	}
	return s->contents;
}
float d_popfloat() {
	if (istack <= 0) {
		__debugbreak();
		_log("stack underflow");
		return 0;
	}
	if (stack[istack-1].k == VINT) {
		// _log("unsafe data cast");
		return (float) stack[-- istack].i;
	} else {
		if (stack[istack-1].k != VFLOAT) {
			_log("invalid data type");
		}
		return stack[-- istack].f;
	}
}
int d_popint() {
	if (istack <= 0) {
		__debugbreak();
		_log("stack underflow");
		return 0;
	}
	if (stack[istack-1].k == VFLOAT) {
		// _log("unsafe data cast");
		return (int) stack[-- istack].f;
	} else {
		if (stack[istack-1].k != VINT) {
			_log("invalid data type");
		}
		return stack[-- istack].i;
	}
}
