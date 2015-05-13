/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MemberInvocationBase.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_METHOD_INVOCATION_BASE
#define HEADER_AST_METHOD_INVOCATION_BASE

#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include "node.h"
#include "ClassSpaceSymbolTable.h"
#include "ast/MethodSegment.h"

namespace wake {

	namespace ast {

		class MethodInvocationBase : public ExpressionNode {

			public:
				MethodInvocationBase(std::vector<MethodSegment*> methodSegments, Node* node, ClassSpaceSymbolTable* classestable, ErrorTracker* errors)
				: errors(errors), node(node), classestable(classestable) {
					for(std::vector<MethodSegment*>::iterator it = methodSegments.begin(); it != methodSegments.end(); ++it) this->methodSegments.push_back(*it);
				}

				virtual PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier)=0;
				virtual ~MethodInvocationBase(){};
				PureType<wake::QUALIFIED>* typeCheckMethodInvocation(PureType<wake::QUALIFIED>& subject);

			protected:
				boost::ptr_vector<MethodSegment> methodSegments;
				ErrorTracker* errors;
				ClassSpaceSymbolTable* classestable;
				Node* node;

		};

	}

}

#endif
