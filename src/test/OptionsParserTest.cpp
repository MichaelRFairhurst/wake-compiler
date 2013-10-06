#include "OptionsParser.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(OptionsParserTest)

BOOST_AUTO_TEST_CASE(TestDefaults) {
	OptionsParser p;
	char wake[5] = "wake";
	char* args[1] = {wake};

	Options* options = p.parse(1, args);

	BOOST_CHECK(options->showHelp == false);
	BOOST_CHECK(options->showVersion == false);
	BOOST_CHECK(options->hasErrors == false);
	BOOST_CHECK(options->listMains == false);
	BOOST_CHECK(options->infilename == "");
	BOOST_CHECK(options->outfilename == "a.out");
	BOOST_CHECK(options->mainclass == "Main");
	BOOST_CHECK(options->mainmethod == "Main");
	delete options;
}

BOOST_AUTO_TEST_CASE(TestShowHelpLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char help[7] = "--help";
	char* args[2] = {wake, help};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->showHelp == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestShowHelpShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char help[3] = "-h";
	char* args[2] = {wake, help};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->showHelp == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestShowVersionLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char version[10] = "--version";
	char* args[2] = {wake, version};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->showVersion == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestShowVersionShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char version[3] = "-v";
	char* args[2] = {wake, version};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->showVersion == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestListMainsLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[12] = "--listmains";
	char* args[2] = {wake, list};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->listMains == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestListMainsShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[3] = "-l";
	char* args[2] = {wake, list};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->listMains == true);
	delete options;
}

BOOST_AUTO_TEST_CASE(TestOutfileLong) {
	OptionsParser p;
	char wake[5] = "wake";
	char out[6] = "--out";
	char file[7] = "myexec";
	char* args[3] = {wake, out, file};

	Options* options = p.parse(3, args);

	BOOST_CHECK(options->outfilename == "myexec");
	delete options;
}

BOOST_AUTO_TEST_CASE(TestOutfileShort) {
	OptionsParser p;
	char wake[5] = "wake";
	char out[3] = "-o";
	char file[7] = "myexec";
	char* args[3] = {wake, out, file};

	Options* options = p.parse(3, args);

	BOOST_CHECK(options->outfilename == "myexec");
	delete options;
}

BOOST_AUTO_TEST_CASE(TestInfileSimple) {
	OptionsParser p;
	char wake[5] = "wake";
	char file[10] = "myfile.wk";
	char* args[2] = {wake, file};

	Options* options = p.parse(2, args);

	BOOST_CHECK(options->infilename == "myfile.wk");
	delete options;
}

BOOST_AUTO_TEST_CASE(TestInfileComplex) {
	OptionsParser p;
	char wake[5] = "wake";
	char list[3] = "-l";
	char out[3] = "-o";
	char outfile[7] = "myexec";
	char file[10] = "myfile.wk";
	char* args[5] = {wake, list, out, outfile, file};

	Options* options = p.parse(5, args);

	BOOST_CHECK(options->infilename == "myfile.wk");
	delete options;
}

BOOST_AUTO_TEST_SUITE_END()
