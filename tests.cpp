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
	test_file.write(string,strlen(string));
	test_file.close();

	std::ifstream input_file;
	input_file.open("test_file");
	size_t size1 = strlen(string);
	size_t size2 = get_infile_length(&input_file);

	char c;
	for(int i=0; i<=3; i++)
		input_file.get(c);

	size_t size3 = get_infile_length(&input_file);

	input_file.close();
	input_file.open("some_random_name_for_file");

	CHECK(size1 == size2);
	CHECK(size2 == size3);
	CHECK(get_infile_length(&input_file)==0);
	input_file.close();
}

TEST_CASE("write_in_file","function test") {


	char char_string[] = "some random string";
	unsigned int len = strlen(char_string);
	unsigned char string[len];
	memcpy(string,char_string,len);
	write_in_file(string,len,"out_file");

	CHECK(!write_in_file(string,1,"."));

	std::ifstream out_file;
	out_file.open("out_file",std::ios::binary);
	CHECK(out_file.is_open());
	CHECK(len == get_infile_length(&out_file));

	char buffer[len];
	out_file.read(buffer,len);
	CHECK(memcmp(buffer,char_string,len)==0);

	out_file.close();
}


TEST_CASE("test everything","compare input and decrypted one"){
	std::ofstream input2;
	input2.open("input",std::ios::binary);
	input2 << "Tohle bude testovaci soubor, tak snad to bude vsechno ok." << std::endl;
	input2.close();

	std::ifstream input;
	std::ofstream output;
	input.open("input",std::ios::binary);
	output.open("encrypted",std::ios::binary);

	aes_encryption(&input,&output);
	input.close();
	output.close();

	input.open("encrypted",std::ios::binary);
	output.open("decrypted",std::ios::binary);
	CHECK(aes_decryption(&input,&output));
}

