#include "TableFileWriter.h"

void TableFileWriter::write(ostream& out, PropertySymbolTable* table) {
	out << "{" << table->classname << "}";
	out << (table->isAbstract() ? "Y" : "N");
	out << "E";
	for(vector<Type*>::iterator it = table->getNeeds()->begin(); it != table->getNeeds()->end(); ++it) {
		writeType(out, *it);
	}
	out << "M";
	for(map<string, ObjectProperty*>::iterator it = table->properties.begin(); it != table->properties.end(); ++it) {
		out << "{" << it->first << "}";
		writeProperty(out, it->second);
	}
}

void TableFileWriter::writeType(ostream& out, Type* type) {
	if(type->type == TYPE_CLASS) {
		out << "T";
		out << "{" << type->typedata._class.classname << "}";
		out << type->typedata._class.shadow << " ";
	} else {
		out << "L";
		if(type->typedata.lambda.returntype != NULL)
			writeType(out, type->typedata.lambda.returntype);

		for(int i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
			writeType(out, type->typedata.lambda.arguments->types[i]);
		}
	}

	out << type->arrayed << " {";
	if(type->alias != NULL) out << type->alias;
	out << "}{";
	if(type->specialty != NULL) out << type->specialty;
	out << "}" << type->optional << " ";
}

void TableFileWriter::writeProperty(ostream& out, ObjectProperty* property) {
	if(property->flags & PROPERTY_PUBLIC) out << "P";
	if(property->flags & PROPERTY_ABSTRACT) out << "B";

	writeType(out, property->type);
}
