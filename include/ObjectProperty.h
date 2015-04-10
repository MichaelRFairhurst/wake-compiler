/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectProperty.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_OBJECT_PROPERTY
#define HEADER_OBJECT_PROPERTY

#include <string>
#include "Annotation.h"

#include "VarDecl.h"

using namespace std;

struct ObjectProperty {
	string address;
	string casing;
	int flags;
	VarDecl decl;
	boost::ptr_vector<Annotation> annotations;
};

#endif
