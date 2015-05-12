/**************************************************
 *
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
	int version = readUInt8(s);
	if(version != 5) throw string("Can not read table file, it has an unsupported version");
	table->setModule(readString(s));
	table->classname = readString(s);
	table->abstract = readUInt8(s);

	int numProperties = readUInt8(s);
	while(numProperties--) {
		readMethod(table, s);
	}
	//cout << "done with props" << endl;

	int numNeeds = readUInt8(s);
	while(numNeeds--) {
		int flags = readUInt8(s);
		SpecializableVarDecl* decl = readSpecializableVarDecl(s);
		vector<Annotation*> anns = readAnnotations(s);
		table->addNeed(decl, flags, anns);
	}

	int numInheritance = readUInt8(s);
	while(numInheritance--) {
		readInheritance(table, s);
	}
	//cout << "done with inheritance" << endl;

	int numParameters = readUInt8(s);
	vector<PureType*>* parameters = new vector<PureType*>();
	while(numParameters--) {
		parameters->push_back(readType(s));
	}
	//cout << "done with parameters" << endl;
	table->setParameters(parameters);
	table->setAnnotations(readAnnotations(s));
}

std::string TableFileReader::readString(istream& s) {
	char* strb = readCString(s);
	if(strb == NULL) return string("");
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

double TableFileReader::readNum64(istream& s) {
	char buffer[4] = {0};
	s.read(buffer, 4);
	double num64 = 0;
	memcpy(buffer, &num64, sizeof(double));
	return num64;
}

PureType* TableFileReader::readType(istream& s) {
	//cout << "reading type" << endl;
	return readTypeByTag(readUInt8(s), s);
}

PureType* TableFileReader::readTypeByTag(int tag, istream& s) {
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

PureType* TableFileReader::readClassType(istream &s) {
	//cout << "reading class type" << endl;
	PureType* type = new PureType(TYPE_CLASS);
	type->typedata._class.modulename = readCString(s);
	type->typedata._class.classname = readCString(s);

	bool hasParams = false;
	int numParams = readUInt8(s);
	while(numParams--) {
		//cout << "reading lambda arg" << endl;
		if(!hasParams) {
			hasParams = true;
			type->typedata._class.parameters = new PureTypeArray();
		}

		addPureTypeToPureTypeArray(readType(s), type->typedata._class.parameters);
	}

	return type;
}

PureType* TableFileReader::readLambdaType(istream& s) {
	//cout << "reading lambda" << endl;
	PureType* type = new PureType(TYPE_LAMBDA);
	type->typedata.lambda.arguments = new PureTypeArray();

	int tag = readUInt8(s);
	if(tag == 0x01) {
		//cout << "reading lambda return" << endl;
		type->typedata.lambda.returntype = readType(s);
	}

	int numArgTypes = readUInt8(s);

	while(numArgTypes--) {
		//cout << "reading lambda arg" << endl;
		addPureTypeToPureTypeArray(readType(s), type->typedata.lambda.arguments);
		tag = readUInt8(s);
	}

	return type;
}

PureType* TableFileReader::readParameterizedType(istream& s, int paramdtype) {
	PureType* type = new PureType(paramdtype);
	type->typedata.parameterized.label = readCString(s);
	int tag = readUInt8(s);
	if(tag != 0x00) {
		type->typedata.parameterized.upperbound = readTypeByTag(tag, s);
	}
	tag = readUInt8(s);
	if(tag != 0x00) {
		type->typedata.parameterized.lowerbound = readTypeByTag(tag, s);
	}
	return type;
}

PureType* TableFileReader::readListType(istream& s) {
	PureType* type = new PureType(TYPE_LIST);
	type->typedata.list.contained = readType(s);
	return type;
}

PureType* TableFileReader::readOptionalType(istream& s) {
	PureType* type = new PureType(TYPE_OPTIONAL);
	type->typedata.optional.contained = readType(s);
	return type;
}

void TableFileReader::readMethod(PropertySymbolTable* table, istream& s) {
	ObjectProperty* prop = new ObjectProperty();
	string name = readString(s);
	prop->address = name; // always true except @TODO when extending generics
	prop->casing = readString(s);
	prop->flags = readUInt8(s);
	prop->decl = *readVarDecl(s);
	vector<Annotation*> annotations = readAnnotations(s);
	for(vector<Annotation*>::iterator ann = annotations.begin(); ann != annotations.end(); ++ann)
		prop->annotations.push_back(*ann);
	table->properties[name] = prop;
	//if(prop->flags & PROPERTY_NEED) table->getNeeds()->push_back(prop->type);
}

VarDecl* TableFileReader::readVarDecl(istream& s) {
	VarDecl* decl = new VarDecl();
	decl->typedata = *readType(s);
	decl->shadow = readUInt8(s);
	decl->alias = readCString(s);
	return decl;
}

SpecializableVarDecl* TableFileReader::readSpecializableVarDecl(istream& s) {
	SpecializableVarDecl* spDecl = new SpecializableVarDecl();
	spDecl->decl = *readVarDecl(s);
	spDecl->specialty = readCString(s);
	return spDecl;
}

void TableFileReader::readInheritance(PropertySymbolTable* table, istream& s) {
	string classname = readString(s);
	table->parentage[classname] = readUInt8(s);;
}

vector<Annotation*> TableFileReader::readAnnotations(istream& s) {
	//cout << "reading annotations" << endl;
	vector<Annotation*> annotations;
	int numAnnotations = readUInt8(s);
	while(numAnnotations--) {
		annotations.push_back(readAnnotation(s));
	}
	//cout << "ending annotations" << endl;
	return annotations;
}

Annotation* TableFileReader::readAnnotation(istream& s) {
	//cout << "reading annotation" << endl;
	Annotation* annotation = new Annotation();
	annotation->name = strdup(readString(s).c_str());

	int numAnnotationVals = readUInt8(s);
	while(numAnnotationVals--) {
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
