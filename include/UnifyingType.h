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

#include "PureType.h"
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	class UnifyingType {

		public:
			UnifyingType(TypeAnalyzer* analyzer) : analyzer(analyzer) {};

			void unifyWith(PureType<wake::QUALIFIED>* unifyWithType);

			PureType<wake::QUALIFIED>* getCurrentUnification();

			PureType<wake::QUALIFIED>* getUnificationFailure1();

			PureType<wake::QUALIFIED>* getUnificationFailure2();

		private:
			std::auto_ptr<PureType<wake::QUALIFIED> > currentUnification;
			std::auto_ptr<PureType<wake::QUALIFIED> > failedToUnify1;
			std::auto_ptr<PureType<wake::QUALIFIED> > failedToUnify2;
			TypeAnalyzer* analyzer;

	};

}

#endif
