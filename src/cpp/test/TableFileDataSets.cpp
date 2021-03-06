/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileDataSets.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TableFileDataSets.h"

											// module  classname length 9     classname   not abstract no properties	no needs	no inheritance	no parameters	no annotations;
char SIMPLE_TABLE[23] = TABLE_FILE_VERSION	"\004test" "\011"                 "classname" "\000"       "\000"			"\000"		"\000"			"\000"			"\000";

														// module   classname length 9     classname   not abstract no properties	no needs	no inheritance	no parameters	no annotations;
char SIMPLE_TABLE_WITH_MODULE[19] = TABLE_FILE_VERSION	"\000" 		"\011"                 "classname" "\000"       "\000"			"\000"		"\000"			"\000"			"\000";

char TABLE_WITH_PUBLIC_METHOD[63] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname length of 9
											"classname"
											"\000" // not abstract
											"\001" // one property
												"\013" // method name length of 11
												"print(Text)"
												"\010" // casing length of 8
												"print(#)"
												"\001" // Flagged public
												"\001" // Lambda
													"\000" // No return
													"\001" // 1 argument
													"\002" // Type
													"\004" // module name ength
													"lang"
													"\004" // class name ength
													"Text"
													"\000" // num params
												"\000" // shadow
												"\000" // alias length
												"\000" // num annotations
											"\000" // num needs
											"\000" // num inheritances
											"\000" // num params
											"\000"; // num  annotations

char TABLE_WITH_SHADOWED_VAR[59] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname length of 9
											"classname"
											"\000" // not abstract
											"\001" // one property
												"\010" // method name length of 8
												"$Printer"
												"\010" // casing length of 8
												"$Printer"
												"\001" // Flagged public
													"\002" // Class type
													"\002" // module
													"io" // module
													"\007" // class name ength
													"Printer"
													"\000" // num params
												"\001" // shadow
												"\000" // alias length
												"\000" // num annotations
											"\000" // num needs
											"\000" // num inheritances
											"\000" // num params
											"\000"; // num  annotations

char TABLE_WITH_ALIASED_VAR[68] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname length of 9
											"classname"
											"\000" // not abstract
											"\001" // one property
												"\010" // method name length of 8
												"mPrinter"
												"\010" // casing length of 8
												"mPrinter"
												"\001" // Flagged public
													"\002" // Class type
													"\002" // module name ength
													"io" // module
													"\007" // class name ength
													"Printer"
													"\000" // num params
												"\000" // shadow
												"\010" // alias length
												"mPrinter" // alias
												"\000" // num annotations
											"\000" // num needs
											"\000" // num inheritances
											"\000" // num params
											"\000"; // num  annotations

char TABLE_WITH_ONE_NEED[47] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname length of 9
											"classname"
											"\000" // not abstract
											"\000" // num properties
											"\001" // num needs
												"\005" // flag public & need
												"\002" // type
												"\004" // module length
												"lang"
												"\004" // classname length
												"Text"
												"\000" // num parameters
												"\000" // shadow
												"\000" // alias
												"\007" // specialty len
												"special" // specialty
												"\000" // num annotations
											"\000" // num inheritance
											"\000" // num parameters
											"\000"; // num annotations

char TABLE_WITH_TWO_NEEDS[70] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname length of 9
											"classname"
											"\000" // not abstract
											"\000" // no properties
											"\002" // two needs
												"\005" // flag public & need
													"\002" // type
													"\004" // module length
													"lang"
													"\004" // classname length
													"Text"
													"\000" // no parameters
												"\000" // shadow
												"\000" // alias length
												"\004" // spec length 8
												"name" // spec
												"\000" // num annotations
												"\005" // flag public & need
													"\002" // type
													"\002" // module length
													"io"
													"\007" // classname length
													"Printer"
													"\000" // parameters
												"\000" // shadow
												"\000" // alias length
												"\010" // spec length 8
												"disabled" // spec
												"\000" // num annotations
											"\000" // num inheritance
											"\000" // num parameters
											"\000"; // num annotations

char TABLE_WITH_INHERITANCE[46] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname len
											"classname"
											"\000" // not abstract
											"\000" // no properties
											"\000" // no needs
											"\002" // two inheritances
											"\013" // parent length 11
											"myinterface"
											"\000" // doesn't extend
											"\010" // parent legnth 8
											"myparent"
											"\001" // does extend
											"\000" // no parameters
											"\000"; // no annotations

char TABLE_WITH_PARAMETERS[55] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname len
											"classname"
											"\000" // not abstract
											"\000" // no props
											"\000" // no needs
											"\000" // no inheritance
											"\002" // 2 parameters
											"\003" // parameterized type
											"\001" "T"
											"\000" // no upper bound
											"\000" // no lower bound
											"\003" // parameterized type
											"\001" "B"
											"\002" // Type upper bound
												"\004" "lang"
												"\004" "Text"
												"\000" // parameters
											"\002" // Type lower bound
												"\004" "lang"
												"\004" "Bool"
												"\000" // parameters
											"\000";// no annotations

char TABLE_WITH_CLASS_ANNOTATIONS[65] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname len
											"classname"
											"\000" // not abstract
											"\000" // no properties
											"\000" // no needs
											"\000" // no inheritance
											"\000" // no parameters
											"\002" // 2 annotations
											"\011" // length 9
											"Annotated"
											"\003" // 3 vals
											"\001" // string val
											"\004" // length 4
											"test"
											"\003" // bool val
											"\000" // false
											"\004" // nothing val
											"\012" // length 10
											"Annotated2"
											"\003" // 3 vals
											"\001" // string val
											"\005" // length 5
											"test2"
											"\003" // bool val
											"\001" // true
											"\004"; // nothing val

char TABLE_WITH_PROPERTY_ANNOTATIONS[86] = TABLE_FILE_VERSION
											"\004" // module len
											"test" // module
											"\011" // classname len
											"classname"
											"\000" // not abstract
											"\001" // 1 properties
												"\006"
												"prop()" // prop name
												"\006"
												"prop()" // prop casing
												"\000" // flags
													"\001" // lambda
													"\000" // no return type
													"\000" // no types
												"\000" // no shadow
												"\000" // no alias
												"\002" // 2 annotations
													"\011" // length 9
													"Annotated"
													"\003" // 3 values
														"\001" // string val
														"\004" // length 4
														"test"
														"\003" // bool val
														"\000" // false
														"\004" // nothing val
													"\012" // length 10
													"Annotated2"
													"\003" // 3 values
														"\001" // string val
														"\005" // length 5
														"test2"
														"\003" // bool val
														"\001" // true
														"\004" // nothing val
											"\000" // no needs
											"\000" // no inheritance
											"\000" // no parameters
											"\000";// no annotations

