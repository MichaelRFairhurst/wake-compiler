#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

FILE* error_file;
char* error_current_filename;
char error_file_buffer[1000] = {0};
const char* error_buffer;
int error_buffer_len = 0;
char error_linebuffer[1000] = {0};
int error_line = 1;
int error_offset = 0;
int error_line_complete = 0;

void error_open_file(const char* filename) {
	error_buffer = &error_file_buffer;
	error_current_filename = filename;
	error_line = 1;
	error_offset = 0;
	error_file = fopen(filename, "r");
	error_line_complete = 0;
}

void error_close_file() {
	fclose(error_file);
}

void error_set_buffer(const char* buffer) {
	error_line_complete = 0;
	error_file = NULL;
	error_buffer = buffer;
	error_buffer_len = strlen(buffer);
	error_line = 1;
	error_offset = 0;
}

void error_read_line(int target) {
	// already have the line
	if(error_line == target && error_line_complete) return;

	// find the line
	while(error_line < target) {
		if(error_offset == error_buffer_len) {
			if(error_file == NULL) break;
			fgets(error_buffer, 1000, error_file);
			error_buffer_len = strlen(error_buffer);
			error_offset = 0;
		}

		while(error_offset < error_buffer_len) {
			if(error_buffer[error_offset++] == '\n') {
				error_line++;
				break;
			}
		}
	}

	int line_start = error_offset;
	int line_len = 0;

	while(error_buffer[error_offset] != '\n' && line_len + error_offset - line_start < 1000) {
		if(error_offset == error_buffer_len) {
			if(error_file == NULL) break;
			memcpy(&error_linebuffer + line_len, error_buffer + line_start, error_buffer_len - line_start);
			line_len += error_buffer_len - line_start;
			line_start = 0;
			fgets(error_buffer, 1000, error_file);
			error_buffer_len = strlen(error_buffer);
			error_offset = 0;
		} else {
			error_offset++;
		}
	}

	error_line_complete = 1;
	memcpy(&error_linebuffer + line_len, error_buffer + line_start, error_offset - line_start);
	error_linebuffer[line_len + error_offset - line_start] = 0;
}

void error_print_multiline(YYLTYPE* location) {
	printf("%*cv== from here\n", location->first_column, ' ');
	int i;
	for(i = location->first_line; i <= location->last_line; i++) {
		if(i - 2 > location->first_line && i + 2 < location->last_line) {
			puts(" ...");
			i = location->last_line - 2;
		}
		error_read_line(i);
		printf("%s\n", error_linebuffer);
	}
	printf("%*c^== until here\n", location->last_column, ' ');
}

void error_print_single_line(YYLTYPE* location) {
	error_read_line(location->first_line);
	putchar('\n');
	puts(error_linebuffer);
	printf("%*c%.*s\n", location->first_column + 1, ' ', location->last_column - location->first_column, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
}

void error_print_error_token(YYLTYPE* location) {
	if(location->first_line < error_line) {
		if(error_file != NULL) {
			fseek(error_file, 0, SEEK_SET);
			error_buffer_len = 0;
			error_offset = 0;
			error_line = 1;
		} else {
			error_offset = 0;
			error_line = 1;
		}
	}

	if(location->first_line != location->last_line) {
		error_print_multiline(location);
	} else {
		error_print_single_line(location);
	}
}
