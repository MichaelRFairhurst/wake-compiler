/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PrecedenceEnforcer.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PRECEDENCE_ENFORCER
#define HEADER_PRECEDENCE_ENFORCER

#include "CompileTargetPrecedenceConfig.h"

class PrecedenceEnforcer {

	public:
		PrecedenceEnforcer(CompileTargetPrecedenceConfig& config) : config(config) {};
		void enforceAssociative(Node* tree, int index);
		void enforceExact(Node* tree, int index, int outer);
		void enforceNonAssociative(Node* tree, int outer);
		void enforce(Node* tree);

	private:
		CompileTargetPrecedenceConfig& config;

};

#endif
