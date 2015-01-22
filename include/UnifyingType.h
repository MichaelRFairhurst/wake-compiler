/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * UnifyingType.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_UNIFYING_TYPE
#define HEADER_UNIFYING_TYPE

#include "type.h"
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	class UnifyingType {

		public:
			UnifyingType(TypeAnalyzer* analyzer) : analyzer(analyzer) {};

			void unifyWith(Type* unifyWithType);

			Type* getCurrentUnification();

			Type* getUnificationFailure1();

			Type* getUnificationFailure2();

		private:
			std::auto_ptr<Type> currentUnification;
			std::auto_ptr<Type> failedToUnify1;
			std::auto_ptr<Type> failedToUnify2;
			TypeAnalyzer* analyzer;

	};

}

#endif
