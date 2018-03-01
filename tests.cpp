//
// Created by sGenn on 28.02.2018.
//


#include "func.hpp"
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

TEST_CASE("Hello word","hello") {
	CHECK(true);
	CHECK(1!=0);
}

TEST_CASE("get_infile_length","function test") {
	std::ofstream test_file;
	test_file.open("test_file",std::ios::binary);
	char string[]="This will be written in file.";
	test_file.write(string,string.size());
	test_file.close();

	std::ifstream input_file;
	input_file.open("test_file");
	size_t size1 = string.size();
	size_t size2 = get_infile_length(input_file);
	CHECK(size1 == size2);
}

