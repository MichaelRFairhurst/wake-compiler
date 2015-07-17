/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * CompileTargetPrecedenceConfig.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_COMPILE_TARGET_PRECEDENCE_CONFIG
#define HEADER_COMPILE_TARGET_PRECEDENCE_CONFIG

#include "tree.h"
#include <map>

class CompileTargetPrecedenceConfig {

	public:
		CompileTargetPrecedenceConfig(std::map<int, int> brain) : brain(brain) {};
		// eventually these can be pure virtual functions
		bool binAssoc(int type);
		bool triAssocLeft(int type);
		bool triAssocMiddle(int type);
		bool triAssocRight(int type);
		bool hasPrec(int type);
		int getPrec(int type);

	private:
		std::map<int, int> brain;

};

#endif
