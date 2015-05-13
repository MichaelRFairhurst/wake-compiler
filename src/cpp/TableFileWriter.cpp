/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileWriter.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TableFileWriter.h"
#include "PureTypeArray.h"

using namespace wake;

void TableFileWriter::write(ostream& out, PropertySymbolTable* table) {
	char dataptr[1] = {0};

	dataptr[0] = (char) 5; //version
	out.write(dataptr, 1);

	dataptr[0] = (char) table->getModule().size();
	out.write(dataptr, 1);
	out.write(table->getModule().c_str(), table->getModule().size());

	dataptr[0] = (char) table->classname.size();
	out.write(dataptr, 1);
	out.write(table->classname.c_str(), table->classname.size());

	dataptr[0] = (char) table->isAbstract();
	out.write(dataptr, 1);

	dataptr[0] = table->properties.size() - table->getNeeds()->size();
	out.write(dataptr, 1);

	for(map<string, ObjectProperty*>::iterator it = table->properties.begin(); it != table->properties.end(); ++it) {
		if(it->second->flags & PROPERTY_NEED) continue;
		dataptr[0] = (char) it->first.size();
		out.write(dataptr, 1);
		out.write(it->first.c_str(), it->first.size());

		writeProperty(out, it->second);
	}

	dataptr[0] = table->getNeeds()->size();
	out.write(dataptr, 1);

	// Its very important that our methods section has our needs listed in order!
	for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator need = table->getNeeds()->begin(); need != table->getNeeds()->end(); ++need) {
		for(map<string, ObjectProperty*>::iterator it = table->properties.begin(); it != table->properties.end(); ++it) {
			if((*need)->decl.createVarRef().toString() != it->second->decl.createVarRef().toString()) continue;
			dataptr[0] = (char) it->second->flags;
			out.write(dataptr, 1);

			writeVarDecl(out, (*need)->decl);

			if((*need)->specialty != NULL) {
				dataptr[0] = (char) strlen((*need)->specialty);
				out.write(dataptr, 1);

				out.write((*need)->specialty, strlen((*need)->specialty));
			} else {
				dataptr[0] = 0x00;
				out.write(dataptr, 1);
			}
			writeAnnotations(out, it->second->annotations);
		}
	}

	dataptr[0] = (char) table->parentage.size();
	out.write(dataptr, 1);

	for(map<string, bool>::iterator it = table->parentage.begin(); it != table->parentage.end(); ++it) {
		dataptr[0] = (char) it->first.size();
		out.write(dataptr, 1);
		out.write(it->first.c_str(), it->first.size());

		dataptr[0] = (char) it->second;
		out.write(dataptr, 1);
	}

	dataptr[0] = table->getParameters().size();
	out.write(dataptr, 1);
	for(vector<PureType<QUALIFIED>*>::const_iterator it = table->getParameters().begin(); it != table->getParameters().end(); ++it) {
		writeType(out, *it);
	}

	writeAnnotations(out, table->getAnnotations());
}

void TableFileWriter::writeType(ostream& out, PureType<QUALIFIED>* type) {
	char dataptr[1] = {0};
	if(type->type == TYPE_CLASS) {
		dataptr[0] = TYPE_CLASS;
		out.write(dataptr, 1);

		if(type->typedata._class.modulename == NULL) {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		} else {
			dataptr[0] = (char) strlen(type->typedata._class.modulename);
			out.write(dataptr, 1);

			out.write(type->typedata._class.modulename, strlen(type->typedata._class.modulename));
		}

		dataptr[0] = (char) strlen(type->typedata._class.classname);
		out.write(dataptr, 1);

		out.write(type->typedata._class.classname, strlen(type->typedata._class.classname));

		if(type->typedata._class.parameters != NULL) {
			dataptr[0] = type->typedata._class.parameters->typecount;
			out.write(dataptr, 1);
			for(int i = 0; i < type->typedata._class.parameters->typecount; i++) {
				writeType(out, type->typedata._class.parameters->types[i]);
			}
		} else {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		}
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

		if(type->typedata.lambda.arguments) {
			dataptr[0] = type->typedata.lambda.arguments->typecount;
			out.write(dataptr, 1);
			for(int i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
				writeType(out, type->typedata.lambda.arguments->types[i]);
			}
		} else {
			dataptr[0] = 0x00;
			out.write(dataptr, 1);
		}
	} else if(type->type == TYPE_PARAMETERIZED || type->type == TYPE_PARAMETERIZED_ARG) {
		dataptr[0] = type->type;
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
	} else if(type->type == TYPE_LIST) {
		dataptr[0] = TYPE_LIST;
		out.write(dataptr, 1);
		writeType(out, type->typedata.list.contained);
	} else if(type->type == TYPE_OPTIONAL) {
		dataptr[0] = TYPE_OPTIONAL;
		out.write(dataptr, 1);
		writeType(out, type->typedata.optional.contained);
	}
}

void TableFileWriter::writeProperty(ostream& out, ObjectProperty* property) {
	char dataptr[1] = {0};

	dataptr[0] = (char) property->casing.size();
	out.write(dataptr, 1);
	out.write(property->casing.c_str(), property->casing.size());

	dataptr[0] = (char) property->flags;
	out.write(dataptr, 1);

	writeVarDecl(out, property->decl);

	writeAnnotations(out, property->annotations);
}

void TableFileWriter::writeVarDecl(ostream& out, VarDecl<QUALIFIED>& decl) {
	writeType(out, &decl.typedata);

	char dataptr[1] = {0};
	dataptr[0] = (char) decl.shadow;
	out.write(dataptr, 1);

	if(decl.alias != NULL) {
		dataptr[0] = (char) strlen(decl.alias);
		out.write(dataptr, 1);

		out.write(decl.alias, strlen(decl.alias));
	} else {
		dataptr[0] = 0x00;
		out.write(dataptr, 1);
	}
}

void TableFileWriter::writeAnnotations(ostream& out, const boost::ptr_vector<Annotation>& annotations) {
	char dataptr[1];
	dataptr[0] = annotations.size();
	out.write(dataptr, 1);

	for(boost::ptr_vector<Annotation>::const_iterator ann = annotations.begin(); ann != annotations.end(); ++ann) {
		dataptr[0] = (char) strlen(ann->name);
		out.write(dataptr, 1);
		out.write(ann->name, strlen(ann->name));

		dataptr[0] = (char) ann->vals.size();
		out.write(dataptr, 1);
		for(boost::ptr_vector<AnnotationVal>::const_iterator val = ann->vals.begin(); val != ann->vals.end(); ++val) {
			dataptr[0] = (char) val->type;
			out.write(dataptr, 1);

			switch(val->type) {
				case ANNOTATION_VAL_TYPE_TEXT:
					dataptr[0] = (char) strlen(val->valdata.text);
					out.write(dataptr, 1);
					out.write(val->valdata.text, strlen(val->valdata.text));
					break;

				case ANNOTATION_VAL_TYPE_BOOL:
					dataptr[0] = (char) (bool) val->valdata.num;
					out.write(dataptr, 1);
					break;

				case ANNOTATION_VAL_TYPE_NUM:
					memcpy((void*) &val->valdata.num, dataptr, sizeof(double));
					out.write(dataptr, sizeof(double));
					break;
			}
		}
	}
}
