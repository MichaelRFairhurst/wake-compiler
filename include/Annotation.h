/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * annotation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_ANNOTATION
#define HEADER_ANNOTATION

#include <boost/ptr_container/ptr_vector.hpp>

#define ANNOTATION_VAL_TYPE_TEXT 1
#define ANNOTATION_VAL_TYPE_NUM 2
#define ANNOTATION_VAL_TYPE_BOOL 3
#define ANNOTATION_VAL_TYPE_NOTHING 4

struct AnnotationVal {
	int type;
	union {
		char* text;
		float num;
	} valdata;
	~AnnotationVal() {
		if(type == ANNOTATION_VAL_TYPE_TEXT) free(valdata.text);
	}
};

struct Annotation {
	char* name;
	boost::ptr_vector<AnnotationVal> vals;
};

#endif
