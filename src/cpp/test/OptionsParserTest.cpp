/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OptionsParserTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "OptionsParser.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(OptionsParserTest)

BOOST_AUTO_TEST_CASE(TestDefaults) {
	OptionsParser p;
	char wake[5] = "wake";
	char* args[1] = {wake};

	Options options = p.parse(1, args);

	BOOST_CHECK(options.showHelp == false);
	BOOST_CHECK(options.showVersion == false);
	BOOST_CHECK(options.hasErrors == false);
	BOOST_CHECK(options.listMains == false);
	BOOST_CHECK(options.link == false);
	BOOST_CHECK(options.table == false);
	BOOST_CHECK(options.tabledir == ".");
	BOOST_CHECK(options.linkFilenames.size() == 0);
	BOOST_CHECK(options.compileFilename == "");
	BOOST_CHECK(options.outFilename == "a.out");
	BOOST_CHECK(options.mainclass == "Main");
	BOOST_CHECK(options.mainmethod == "main()");
}

BOOST_AUTO_TEST_CASE(TestShowHelpLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char help[7] = "--help";
	char* args[2] = {wake, help};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.showHelp == true);
}

BOOST_AUTO_TEST_CASE(TestShowHelpShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char help[3] = "-h";
	char* args[2] = {wake, help};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.showHelp == true);
}

BOOST_AUTO_TEST_CASE(TestShowVersionLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char version[10] = "--version";
	char* args[2] = {wake, version};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.showVersion == true);
}

BOOST_AUTO_TEST_CASE(TestShowVersionShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char version[3] = "-v";
	char* args[2] = {wake, version};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.showVersion == true);
}

BOOST_AUTO_TEST_CASE(TestListMainsLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[12] = "--listmains";
	char* args[2] = {wake, list};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.listMains == true);
}

BOOST_AUTO_TEST_CASE(TestListMainsShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[3] = "-i";
	char* args[2] = {wake, list};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.listMains == true);
}

BOOST_AUTO_TEST_CASE(TestOutfileLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char out[6] = "--out";
	char file[7] = "myexec";
	char* args[3] = {wake, out, file};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.outFilename == "myexec");
}

BOOST_AUTO_TEST_CASE(TestOutfileShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char out[3] = "-o";
	char file[7] = "myexec";
	char* args[3] = {wake, out, file};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.outFilename == "myexec");
}

BOOST_AUTO_TEST_CASE(TestInfileSimple) {
	OptionsParser p;
	char wake[5] = "wake";
	char file[10] = "myfile.wk";
	char* args[2] = {wake, file};

	Options options = p.parse(2, args);

	BOOST_CHECK(options.compileFilename == "myfile.wk");
}

BOOST_AUTO_TEST_CASE(TestInfileComplex) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[3] = "-i";
	char out[3] = "-o";
	char outfile[7] = "myexec";
	char file[10] = "myfile.wk";
	char* args[5] = {wake, list, out, outfile, file};

	Options options = p.parse(5, args);

	BOOST_CHECK(options.compileFilename == "myfile.wk");
}

BOOST_AUTO_TEST_CASE(TestLinkOneFile) {
	OptionsParser p;
	char wake[5] = "wake";
	char link[7] = "--link";
	char file[9] = "myfile.o";
	char* args[3] = {wake, link, file};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.link == true);
	BOOST_CHECK(options.linkFilenames.size() == 1);
	BOOST_CHECK(options.compileFilename == "");
	BOOST_CHECK(options.linkFilenames[0] == "myfile.o");
}

BOOST_AUTO_TEST_CASE(TestLinkTwoFiles) {
	OptionsParser p;
	char wake[5] = "wake";
	char link[7] = "--link";
	char file1[9] = "myfile.o";
	char file2[8] = "file2.o";
	char* args[4] = {wake, link, file1, file2};

	Options options = p.parse(4, args);

	BOOST_CHECK(options.link == true);
	BOOST_CHECK(options.linkFilenames.size() == 2);
	BOOST_CHECK(options.compileFilename == "");
	BOOST_CHECK(options.linkFilenames[0] == "myfile.o");
	BOOST_CHECK(options.linkFilenames[1] == "file2.o");
}

BOOST_AUTO_TEST_CASE(TestTableFileLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char table[8] = "--table";
	char file[10] = "myfile.wk";
	char* args[3] = {wake, table, file};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.link == false);
	BOOST_CHECK(options.table == true);
	BOOST_CHECK(options.linkFilenames.size() == 0);
	BOOST_CHECK(options.compileFilename == "myfile.wk");
	BOOST_CHECK(options.outFilename == "a.out");
}

BOOST_AUTO_TEST_CASE(TestTableFileShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char table[3] = "-t";
	char file[10] = "myfile.wk";
	char* args[3] = {wake, table, file};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.link == false);
	BOOST_CHECK(options.table == true);
	BOOST_CHECK(options.linkFilenames.size() == 0);
	BOOST_CHECK(options.compileFilename == "myfile.wk");
	BOOST_CHECK(options.outFilename == "a.out");
}

BOOST_AUTO_TEST_CASE(TestTableDir) {
	OptionsParser p;
	char wake[5] = "wake";
	char tabledir[11] = "--tabledir";
	char name[9] = "tabledir";
	char* args[3] = {wake, tabledir, name};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.link == false);
	BOOST_CHECK(options.table == false);
	BOOST_CHECK(options.tabledir == "tabledir");
	BOOST_CHECK(options.linkFilenames.size() == 0);
	BOOST_CHECK(options.compileFilename == "");
	BOOST_CHECK(options.outFilename == "a.out");
}

BOOST_AUTO_TEST_CASE(TestTableDirShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char tabledir[3] = "-d";
	char name[9] = "tabledir";
	char* args[3] = {wake, tabledir, name};

	Options options = p.parse(3, args);

	BOOST_CHECK(options.link == false);
	BOOST_CHECK(options.table == false);
	BOOST_CHECK(options.tabledir == "tabledir");
	BOOST_CHECK(options.linkFilenames.size() == 0);
	BOOST_CHECK(options.compileFilename == "");
	BOOST_CHECK(options.outFilename == "a.out");
}

BOOST_AUTO_TEST_SUITE_END()
