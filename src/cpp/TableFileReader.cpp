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

using namespace wake;

void TableFileReader::read(PropertySymbolTable* table, istream& s) {
	int version = readUInt8(s);
	if(version != 6) throw string("Can not read table file, it has an unsupported version");
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
		SpecializableVarDecl<QUALIFIED>* decl = readSpecializableVarDecl(s);
		vector<Annotation*> anns = readAnnotations(s);
		table->addNeed(decl, flags, anns);
	}

	int numInheritance = readUInt8(s);
	while(numInheritance--) {
		readInheritance(table, s);
	}
	//cout << "done with inheritance" << endl;

	int numParameters = readUInt8(s);
	vector<PureType<QUALIFIED>*>* parameters = new vector<PureType<QUALIFIED>*>();
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

PureType<QUALIFIED>* TableFileReader::readType(istream& s) {
	//cout << "reading type" << endl;
	return readTypeByTag(readUInt8(s), s);
}

PureType<QUALIFIED>* TableFileReader::readTypeByTag(int tag, istream& s) {
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

PureType<QUALIFIED>* TableFileReader::readClassType(istream &s) {
	//cout << "reading class type" << endl;
	PureType<QUALIFIED>* type = new PureType<QUALIFIED>(TYPE_CLASS);
	type->typedata._class.modulename = readCString(s);
	type->typedata._class.classname = readCString(s);

	bool hasParams = false;
	int numParams = readUInt8(s);
	while(numParams--) {
		//cout << "reading lambda arg" << endl;
		if(!hasParams) {
			hasParams = true;
			type->typedata._class.parameters = new PureTypeArray<QUALIFIED>();
		}

		type->typedata._class.parameters->addType(readType(s));
	}

	return type;
}

PureType<QUALIFIED>* TableFileReader::readLambdaType(istream& s) {
	//cout << "reading lambda" << endl;
	PureType<QUALIFIED>* type = new PureType<QUALIFIED>(TYPE_LAMBDA);
	type->typedata.lambda.arguments = new PureTypeArray<QUALIFIED>();

	int tag = readUInt8(s);
	if(tag == 0x01) {
		//cout << "reading lambda return" << endl;
		type->typedata.lambda.returntype = readType(s);
	}

	int numArgTypes = readUInt8(s);

	while(numArgTypes--) {
		//cout << "reading lambda arg" << endl;
		type->typedata.lambda.arguments->addType(readType(s));
	}

	return type;
}

PureType<QUALIFIED>* TableFileReader::readParameterizedType(istream& s, int paramdtype) {
	PureType<QUALIFIED>* type = new PureType<QUALIFIED>(paramdtype);
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

PureType<QUALIFIED>* TableFileReader::readListType(istream& s) {
	PureType<QUALIFIED>* type = new PureType<QUALIFIED>(TYPE_LIST);
	type->typedata.list.contained = readType(s);
	return type;
}

PureType<QUALIFIED>* TableFileReader::readOptionalType(istream& s) {
	PureType<QUALIFIED>* type = new PureType<QUALIFIED>(TYPE_OPTIONAL);
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

VarDecl<QUALIFIED>* TableFileReader::readVarDecl(istream& s) {
	VarDecl<QUALIFIED>* decl = new VarDecl<QUALIFIED>();
	decl->typedata = *readType(s);
	decl->shadow = readUInt8(s);
	decl->alias = readCString(s);
	return decl;
}

SpecializableVarDecl<QUALIFIED>* TableFileReader::readSpecializableVarDecl(istream& s) {
	SpecializableVarDecl<QUALIFIED>* spDecl = new SpecializableVarDecl<QUALIFIED>();
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
