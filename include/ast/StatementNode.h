/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * StatementNode.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_STATEMENT_NODE
#define HEADER_AST_STATEMENT_NODE

namespace wake {

	namespace ast {

		class StatementNode {

			public:
				virtual void typeCheck()=0;

		};

	}

}

#endif
