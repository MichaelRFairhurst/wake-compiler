/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileReader.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TableFileReader.h"
#include "ObjectProperty.h"
#include <iostream>

void TableFileReader::read(PropertySymbolTable* table, istream& s) {
	//@TODO work rom tablefile
	int version = readUInt8(s);
	if(version < 3 || version > 4) throw string("Can not read table file, it has an unsupported version");
	table->classname = readString(s);
	table->abstract = readUInt8(s);
	unsigned char tag;
	while(true) {
		tag = readUInt8(s);
		if(tag == 0x00) break;
		s.putback(tag);// TODO make tag an argument on readMethod
		readMethod(table, s);
	}
	//cout << "done with props" << endl;

	while(true) {
		tag = readUInt8(s);
		if(tag == 0x00) break;
		s.putback(tag);// TODO make tag an argument on readInheritance
		readInheritance(table, s);
	}
	//cout << "done with inheritance" << endl;

	vector<Type*>* parameters = new vector<Type*>();
	while(true) {
		tag = readUInt8(s);
		if(tag == 0x00) break;
		s.putback(tag);// TODO make tag an argument on readInheritance
		parameters->push_back(readType(s)); // @TODO read parameterized types
	}
	//cout << "done with parameters" << endl;
	table->setParameters(parameters);
	table->setAnnotations(readAnnotations(s));
}

std::string TableFileReader::readString(istream& s) {
	char* strb = readCString(s);
	if(strb == NULL) return NULL;
	std::string str(strb);
	free(strb);
	return str;
}

char* TableFileReader::readCString(istream& s) {
	unsigned char len = readUInt8(s);
	if(len == 0) {
		//cout << "Read string NULL" << endl;
		return NULL;
	}

	char* strb = (char*) malloc(len + 1);
	s.read(strb, len);
	strb[len] = 0;
	//cout << "Read string " << strb << endl;
	return strb;
}

unsigned char TableFileReader::readUInt8(istream& s) {
	char* uint8p = (char*) malloc(1);
	s.read(uint8p, 1);
	unsigned char uint8 = *uint8p;
	free(uint8p);
	//cout << "Read uint8 " << ((int) uint8) << endl;
	return uint8;
}

float TableFileReader::readNum64(istream& s) {
	char buffer[4] = {0};
	s.read(buffer, 4);
	float num64 = 0;
	memcpy(buffer, &num64, sizeof(float));
	return num64;
}

Type* TableFileReader::readType(istream& s) {
	//cout << "reading type" << endl;
	return readTypeByTag(readUInt8(s), s);
}

Type* TableFileReader::readTypeByTag(int tag, istream& s) {
	//cout << "reading type by tag " << tag << endl;
	if(tag == TYPE_CLASS) {
		return readClassType(s);
	} else if(tag == TYPE_LAMBDA) {
		return readLambdaType(s);
	} else if(tag == TYPE_PARAMETERIZED || tag == TYPE_PARAMETERIZED_ARG) {
		return readParameterizedType(s, tag);
	} else if(tag == TYPE_LIST) {
		return readListType(s);
	} else if(tag == TYPE_OPTIONAL) {
		return readOptionalType(s);
	}
}

Type* TableFileReader::readClassType(istream &s) {
	//cout << "reading class type" << endl;
	Type* type = MakeType(TYPE_CLASS);
	type->typedata._class.classname = readCString(s);

	bool hasParams = false;
	int tag = readUInt8(s);
	while(tag != 0x00) {
		//cout << "reading lambda arg" << endl;
		if(!hasParams) {
			hasParams = true;
			type->typedata._class.parameters = MakeTypeArray();
		}

		AddTypeToTypeArray(readTypeByTag(tag, s), type->typedata._class.parameters);
		tag = readUInt8(s);
	}

	type->typedata._class.shadow = readUInt8(s);

	readTypeCommon(type, s);

	return type;
}

Type* TableFileReader::readLambdaType(istream& s) {
	//cout << "reading lambda" << endl;
	Type* type = MakeType(TYPE_LAMBDA);
	type->typedata.lambda.arguments = MakeTypeArray();

	int tag = readUInt8(s);
	if(tag == 0x01) {
		//cout << "reading lambda return" << endl;
		type->typedata.lambda.returntype = readType(s);
	}
	tag = readUInt8(s);

	while(tag != 0x00) {
		//cout << "reading lambda arg" << endl;
		AddTypeToTypeArray(readTypeByTag(tag, s), type->typedata.lambda.arguments);
		tag = readUInt8(s);
	}

	readTypeCommon(type, s);

	return type;
}

Type* TableFileReader::readParameterizedType(istream& s, int paramdtype) {
	Type* type = MakeType(paramdtype);
	type->typedata.parameterized.label = readCString(s);
	int tag = readUInt8(s);
	if(tag != 0x00) {
		type->typedata.parameterized.upperbound = readTypeByTag(tag, s);
	}
	tag = readUInt8(s);
	if(tag != 0x00) {
		type->typedata.parameterized.lowerbound = readTypeByTag(tag, s);
	}
	type->typedata.parameterized.shadow = readUInt8(s);

	readTypeCommon(type, s);
	return type;
}

Type* TableFileReader::readListType(istream& s) {
	Type* type = MakeType(TYPE_LIST);
	type->typedata.list.contained = readType(s);
	readUInt8(s); // backwards compat

	readTypeCommon(type, s);
	return type;
}

Type* TableFileReader::readOptionalType(istream& s) {
	Type* type = MakeType(TYPE_OPTIONAL);
	type->typedata.optional.contained = readType(s);
	readUInt8(s); // backwards compat

	readTypeCommon(type, s);
	return type;
}

void TableFileReader::readTypeCommon(Type* type, istream& s) {
	//cout << "reading type common" << endl;
	type->alias = readCString(s);
	type->specialty = readCString(s);
	//cout << "done reading type" << endl;
}

void TableFileReader::readMethod(PropertySymbolTable* table, istream& s) {
	ObjectProperty* prop = new ObjectProperty();
	string name = readString(s);
	prop->address = name; // always true except @TODO when extending generics
	prop->casing = readString(s);
	prop->flags = readUInt8(s);
	prop->type = readType(s);
	vector<Annotation*> annotations = readAnnotations(s);
	for(vector<Annotation*>::iterator ann = annotations.begin(); ann != annotations.end(); ++ann)
		prop->annotations.push_back(*ann);
	table->properties[name] = prop;
	if(prop->flags & PROPERTY_NEED) table->getNeeds()->push_back(prop->type);
}

void TableFileReader::readInheritance(PropertySymbolTable* table, istream& s) {
	string classname = readString(s);
	table->parentage[classname] = readUInt8(s);;
}

vector<Annotation*> TableFileReader::readAnnotations(istream& s) {
	//cout << "reading annotations" << endl;
	vector<Annotation*> annotations;
	while(true) {
		int tag = readUInt8(s);
		if(tag == 0) break;

		s.putback(tag);
		annotations.push_back(readAnnotation(s));
	}
	//cout << "ending annotations" << endl;
	return annotations;
}

Annotation* TableFileReader::readAnnotation(istream& s) {
	//cout << "reading annotation" << endl;
	Annotation* annotation = new Annotation();
	annotation->name = strdup(readString(s).c_str());

	while(true) {
		//cout << "reading annotation val" << endl;
		int tag = readUInt8(s);
		if(tag == 0) break;

		AnnotationVal* val = new AnnotationVal;
		val->type = tag;

		if(tag == ANNOTATION_VAL_TYPE_TEXT) {
			val->valdata.text = readCString(s);
		} else if(tag == ANNOTATION_VAL_TYPE_BOOL) {
			val->valdata.num = readUInt8(s);
		} else if(tag == ANNOTATION_VAL_TYPE_NUM) {
			val->valdata.num = readNum64(s);
		} // nothing to do for ANNOTATION_VAL_TYPE_NOTHING

		annotation->vals.push_back(val);
	}

	//cout << "end annotation" << endl;
	return annotation;
}
