#include "TableFileReader.h"

void TableFileReader::read(PropertySymbolTable* table, istream& s) {
	std::string classname = readString(s);
	unsigned char tag;
	while(tag = readUInt8(s)) {
		table->addNeed(readTypeByTag(tag, s));
	}

	while(true) {
		s.peek(); // trigger EOF
		if(s.eof()) break;
		readMethod(table, s);
	}
}

std::string TableFileReader::readString(istream& s) {
	char* strb = readCString(s);
	std::string str(strb);
	free(strb);
	return str;
}

char* TableFileReader::readCString(istream& s) {
	unsigned char len = readUInt8(s);
	if(len == 0) return NULL;

	char* strb = (char*) malloc(len + 1);
	s.read(strb, len);
	strb[len] = 0;
	return strb;
}

unsigned char TableFileReader::readUInt8(istream& s) {
	char* uint8p = (char*) malloc(1);
	s.read(uint8p, 1);
	unsigned char uint8 = *uint8p;
	free(uint8p);
	return uint8;
}

Type* TableFileReader::readType(istream& s) {
	return readTypeByTag(readUInt8(s), s);
}

Type* TableFileReader::readTypeByTag(int tag, istream& s) {
	return tag == 0x01 ? readClassType(s) : readLambdaType(s);
}

Type* TableFileReader::readClassType(istream &s) {
	Type* type = MakeType(TYPE_CLASS);
	type->typedata._class.classname = readCString(s);
	type->typedata._class.shadow = readUInt8(s);

	readTypeCommon(type, s);

	return type;
}

Type* TableFileReader::readLambdaType(istream& s) {
	Type* type = MakeType(TYPE_LAMBDA);
	type->typedata.lambda.arguments = MakeTypeArray();

	int tag = readUInt8(s);
	if(tag == 0x03) {
		type->typedata.lambda.returntype = readTypeByTag(tag, s);
	}

	while(tag != 0x04) {
		AddTypeToTypeArray(readTypeByTag(tag, s), type->typedata.lambda.arguments);
	}
}

void TableFileReader::readTypeCommon(Type* type, istream& s) {
	type->arrayed = readUInt8(s);
	type->alias = readCString(s);
	type->specialty = readCString(s);
	type->optional = readUInt8(s);
}

void TableFileReader::readMethod(PropertySymbolTable* table, istream& s) {
	string name = readString(s);
	int flags = readUInt8(s);
}
