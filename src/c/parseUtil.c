/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * parseUtil.c
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "parseUtil.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
	if(in[i] == '\\') {
		int increment;
		out[c] = unEscapeCharLiteral(in + i + 1, &increment);
		c++;
		if(increment != -1) {
			i += increment; // one more will be added by the for loop
		} else {
			free(out); free(in);
			return NULL;
		}
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

char unEscapeCharLiteral(char* in, int* count) {
	if(isdigit(in[0])) {
		int i = 1;
		char c = in[0] - '0';
		while(i < 3 && isdigit(in[i])) {
			char increment = in[i] - '0';

			// overflow condition
			if(c * 10 + increment > 255) {
				*count = -1;
				return '\0';
			}

			c = c * 10 + (in[i] - '0');
		}

		*count = i;
		return c;
	}

	char out;
	switch(in[0]) {
		case '\\':
			out = '\\'; break;
		case 'n':
			out = '\n'; break;
		case 't':
			out = '\t'; break;
		case 'r':
			out = '\r'; break;
		case 'b':
			out = '\b'; break;
		case 'f':
			out = '\f'; break;
		case 'v':
			out = '\v'; break;
		case 'a':
			out = '\a'; break;
		case '\'':
		case '"':
			out = in[0]; break;
		default:
			*count = -1;
			return '\0';
	}

	*count = 1;
	return out;
}
