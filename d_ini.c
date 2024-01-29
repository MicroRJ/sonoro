

typedef struct t_exporter {
	FILE *f;
} t_exporter;

typedef struct t_importer {
	char *p;
} t_importer;


void ini_writeheader(t_exporter *e, char const *name) {
	fprintf(e->f,"[%s]\n",name);
}

void ini_writefield(t_exporter *e, char *name) {
	switch (getk()) {
		case VSTRING: {
			char *s = popx();
			if (s != 0) {
				fprintf(e->f,"\t.%s = \"%s\"\n",name,s);
			}
		} break;
		case VFLOAT: {
			fprintf(e->f,"\t.%s = %f\n",name,d_popfloat());
		} break;
		case VINT: {
			fprintf(e->f,"\t.%s = %i\n",name,d_popint());
		} break;
	}
}


int isnumberstarter(char p) {
	return p == '.' || p == '+' || p == '-' || (p >= '0' && p <= '9');
}

int isnumber(char p) {
	return p >= '0' && p <= '9';
}

int isletter(char p) {
	return (p >= 'a' && p <= 'z') || (p >= 'A' && p <= 'Z');
}

int isblankorline(char p) {
	return p == ' ' || p == '\t' || p == '\r' || p == '\n';
}

int isblankortab(char p) {
	return p == ' ' || p == '\t';
}

/* read a name into a buffer, return -1 if invalid otherwise
 returns the length of the name. */
int nextname(t_importer *i, char *b) {
	if (!isletter(*i->p) && *i->p != '_') {
		return -1;
	}
	char *w = b;
	do {
		*w ++ = *i->p ++;
	} while (isletter(*i->p) || isnumber(*i->p) || *i->p == '_' || *i->p == '.');
	*w = '\0';
	return w - b;
}

int ini_nextfield(t_importer *i, char *b) {
	while (isblankorline(*i->p)) ++ i->p;

	if (*i->p != '.') {
		return 0;
	}
	i->p += 1;

	while (isblankortab(*i->p)) ++ i->p;

	int r = nextname(i,b);
	if (r == -1) {
		_log("invalid field name");
	}

	while (isblankortab(*i->p)) ++ i->p;

	if (*i->p != '=') {
		_log("expected '='");
		return 0;
	}
	i->p += 1;

	while (isblankortab(*i->p)) ++ i->p;

	if (isnumberstarter(*i->p)) {
		float f = strtof(i->p,&i->p);
		d_putfloat(f);
	} else
	if (*i->p == '"') {
		i->p += 1;

		char j[MAX_NAME];
		char *w = j;
		while (*i->p != '"' && *i->p != '\0') {
			*w ++ = *i->p ++;
		}
		*w = 0;
		putx(j);

		if (*i->p == '"') {
			i->p += 1;
		} else {
			_log("unterminated string");
		}
	} else {
		_log("invalid field value");
	}
	return 1;
}

int ini_nextheader(t_importer *i, char *b) {
	if (i->p == 0 || *i->p == 0) {
		return 0;
	}
	while (isblankorline(*i->p)) i->p ++;
	if (*i->p != '[') {
		_log("expected '[', got %c",*i->p);
		return 0;
	}
	i->p += 1;
	/* consume header name */
	int r = nextname(i,b);
	if (r == -1) {
		_log("invalid header name");
	}
	while (isblankortab(*i->p)) i->p ++;
	if (*i->p != ']') {
		_log("expected ']'");
		return 0;
	}
	i->p += 1;
	return 1;
}
