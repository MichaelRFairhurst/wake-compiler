#include "TableFileWriter.h"

void TableFileWriter::write(ostream& out, PropertySymbolTable* table) {
	char* dataptr = (char*) malloc(1);

	dataptr[0] = (char) table->classname.size();
	out.write(dataptr, 1);
	out.write(table->classname.c_str(), table->classname.size());

	dataptr[0] = (char) table->isAbstract();
	out.write(dataptr, 1);

	for(vector<Type*>::iterator it = table->getNeeds()->begin(); it != table->getNeeds()->end(); ++it) {
		writeType(out, *it);
	}

	dataptr[0] = 0x00; // End Nededs
	out.write(dataptr, 1);

	for(map<string, ObjectProperty*>::iterator it = table->properties.begin(); it != table->properties.end(); ++it) {
		dataptr[0] = (char) it->first.size();
		out.write(dataptr, 1);
		out.write(it->first.c_str(), it->first.size());

		writeProperty(out, it->second);
	}

	dataptr[0] = 0x00; // End Methods
	out.write(dataptr, 1);

	for(map<string, bool>::iterator it = table->parentage.begin(); it != table->parentage.end(); ++it) {
		dataptr[0] = (char) it->first.size();
		out.write(dataptr, 1);
		out.write(it->first.c_str(), it->first.size());

		dataptr[0] = (char) it->second;
		out.write(dataptr, 1);
	}

	dataptr[0] = 0x00; // End Inheritances
	out.write(dataptr, 1);

	for(auto it = table->getParameters().begin(); it != table->getParameters().end(); ++it) {
		writeType(out, *it);
	}
	free(dataptr);
}

void TableFileWriter::writeType(ostream& out, Type* type) {
	char * dataptr = (char*) malloc(1);
	if(type->type == TYPE_CLASS) {
		dataptr[0] = TYPE_CLASS;
		out.write(dataptr, 1);

		dataptr[0] = (char) strlen(type->typedata._class.classname);
		out.write(dataptr, 1);

		out.write(type->typedata._class.classname, strlen(type->typedata._class.classname));

		if(type->typedata._class.parameters != NULL) {
			for(int i = 0; i < type->typedata._class.parameters->typecount; i++) {
				writeType(out, type->typedata._class.parameters->types[i]);
			}
		}

		dataptr[0] = 0x00;
		out.write(dataptr, 1);

		dataptr[0] = (char) type->typedata._class.shadow;
		out.write(dataptr, 1);
	} else if(type->type == TYPE_LAMBDA) {
		dataptr[0] = TYPE_LAMBDA;
		out.write(dataptr, 1);

		if(type->typedata.lambda.returntype != NULL) {
			dataptr[0] = 0x01;
			out.write(dataptr, 1);

			writeType(out, type->typedata.lambda.returntype);
		} else {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		}

		for(int i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
			writeType(out, type->typedata.lambda.arguments->types[i]);
		}

		dataptr[0] = 0x00;
		out.write(dataptr, 1);
	} else if(type->type == TYPE_PARAMETERIZED) {
		dataptr[0] = TYPE_PARAMETERIZED;
		out.write(dataptr, 1);

		dataptr[0] = (char) strlen(type->typedata.parameterized.label);
		out.write(dataptr, 1);

		out.write(type->typedata.parameterized.label, strlen(type->typedata.parameterized.label));

		if(type->typedata.parameterized.upperbound != NULL) {
			writeType(out, type->typedata.parameterized.upperbound);
		} else {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		}

		if(type->typedata.parameterized.lowerbound != NULL) {
			writeType(out, type->typedata.parameterized.lowerbound);
		} else {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		}

		dataptr[0] = type->typedata.parameterized.shadow;
		out.write(dataptr, 1);
	}

	dataptr[0] = (char) type->arrayed;
	out.write(dataptr, 1);

	if(type->alias != NULL) {
		dataptr[0] = strlen(type->alias);
		out.write(dataptr, 1);
		out.write(type->alias, strlen(type->alias));
	} else {
		dataptr[0] = 0;
		out.write(dataptr, 1);
	}

	if(type->specialty != NULL) {
		dataptr[0] = strlen(type->specialty);
		out.write(dataptr, 1);
		out.write(type->specialty, strlen(type->specialty));
	} else {
		dataptr[0] = 0;
		out.write(dataptr, 1);
	}

	dataptr[0] = type->optional;
	out.write(dataptr, 1);
	free(dataptr);
}

void TableFileWriter::writeProperty(ostream& out, ObjectProperty* property) {
	char * dataptr = (char*) malloc(1);

	dataptr[0] = (char) property->casing.size();
	out.write(dataptr, 1);
	out.write(property->casing.c_str(), property->casing.size());

	dataptr[0] = (char) property->flags;
	out.write(dataptr, 1);

	writeType(out, property->type);
	free(dataptr);
}
