#include "parseUtil.h"
#include <stdlib.h>
#include <string.h>

char* keepInner(char* in) {
	char* out = malloc(sizeof(char)*strlen(in) - 1);
	memcpy(out, &in[1], strlen(in) - 2);
	out[strlen(in) - 2] = '\0';
	return out;
}

char* unEscapeStringLiteral(char* in, char terminator) {
	int i = 0;
	int c = 0;
	char* out = malloc(sizeof(char) * strlen(in) + 1);

	for(i = 0; i < strlen(in); i++)
	if(in[i] == '\\')
	switch(in[i + 1]) {
		case '\\':
			out[c] = '\\';
			c++; i++; break;
		case 'n':
			out[c] = '\n';
			c++; i++; break;
		case 't':
			out[c] = '\t';
			c++; i++; break;
		case 'r':
			out[c] = '\r';
			c++; i++; break;
		case 'b':
			out[c] = '\b';
			c++; i++; break;
		case 'f':
			out[c] = '\f';
			c++; i++; break;
		case 'v':
			out[c] = '\v';
			c++; i++; break;
		default:
			if(in[i + 1] == terminator) continue;
			out[c] = '\\';
			c++;
	}
	else {
		out[c] = in[i];
		c++;
	}

	out[c] = '\0';

	// free some memory
	out = realloc(out, sizeof(char) * c + 1);
	free(in);
	return out;
}
