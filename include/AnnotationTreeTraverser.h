/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AnnotationTreeTraverser.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_ANNOTATION_TREE_TRAVERSER
#define HEADER_ANNOTATION_TREE_TRAVERSER

#include <vector>
#include "tree.h"
#include "Annotation.h"

using namespace std;

class AnnotationTreeTraverser {

	public:
		 vector<Annotation*> getAnnotations(Node* tree);

	private:
		 Annotation* getSingleAnnotation(Node* tree);

};

#endif
