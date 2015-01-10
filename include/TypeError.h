/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeError.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TYPE_ERROR
#define HEADER_TYPE_ERROR

#include <string>

namespace wake {

	class TypeError {

		public:
			TypeError(std::string expected, std::string erroneous, std::string reason) : expected(expected), erroneous(erroneous), reason(reason) {};

			std::string expected;
			std::string erroneous;
			std::string reason;

	};

}

#define EXPECTED throw new TypeError((
#define ERRONEOUS ),(
#define THROW(reason) ),(reason));

#endif
