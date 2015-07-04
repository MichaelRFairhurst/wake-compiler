/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Char.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_CHAR
#define HEADER_AST_CHAR

#include "ExpressionNode.h"
#include "TypeAnalyzer.h"
#include "node.h"
#include "ErrorTracker.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace wake {

	namespace ast {

		class Char : public ExpressionNode {

			public:
				Char(char* theChar, Node* node, ErrorTracker* errors) : theChar(theChar), node(node), errors(errors) {};

				PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier);


			private:
				char* theChar;
				Node* node;
				ErrorTracker* errors;

		};

	}

}

#endif
