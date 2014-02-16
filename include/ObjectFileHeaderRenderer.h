#ifndef HEADER_OBJECT_FILE_HEADER_RENDERER
#define HEADER_OBJECT_FILE_HEADER_RENDERER

#include <iostream>
#include "ObjectFileHeaderData.h"

class ObjectFileHeaderRenderer {

	public:
		void writeOut(std::ostream& out, ObjectFileHeaderData* data);
};

#endif
