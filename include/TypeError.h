#ifndef HEADER_TYPE_ERROR
#define HEADER_TYPE_ERROR

namespace wake {

	class TypeError {

		public:
			TypeError(std::string expected, std::string erroneous, std::string reason) : expected(expected), erroneous(erroneous), reason(reason);

			std::string expected;
			std::string erroneous;
			std::string reason;

	}

}

#define EXPECTED throw new TypeError((
#define ERRONEOUS ),(
#define THROW(reason) ),(reason));

#endif
