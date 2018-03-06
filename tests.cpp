//
// Created by sGenn on 28.02.2018.
//


#include "func.hpp"
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

TEST_CASE("hash_input","tests_vectors") {
	std::ifstream hash_file;
	std::ifstream hash_hex;
	char hash_file_char[65];
	char hash_to_be[65];

	//for test vector "abc", file with check hash is "hex1.txt"
	char test_abc_char[] = "abc";
	unsigned char test_abc[strlen(test_abc_char)];
	memcpy(test_abc,test_abc_char,strlen(test_abc_char));
	hash_input(test_abc,strlen(test_abc_char));

	hash_file.open("hash_file",std::ios::binary);
	hash_file.read(hash_file_char,65);
	hash_file.close();

	hash_hex.open("hex1.txt",std::ios::binary);
	hash_hex.read(hash_to_be,65);
	hash_hex.close();
	CHECK(memcmp(hash_file_char,hash_to_be,64)==0);


	//for test vector "", file with check hash is "hex2.txt"
	unsigned char test_empty[]={};
	hash_input(test_empty,0);

	hash_file.open("hash_file",std::ios::binary);
	hash_file.read(hash_file_char,65);
	hash_file.close();

	hash_hex.open("hex2.txt",std::ios::binary);
	hash_hex.read(hash_to_be,65);
	hash_hex.close();
	CHECK(memcmp(hash_file_char,hash_to_be,64)==0);


//for test vector long, file with check hash is "hex1.txt"
	char test_char[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
	unsigned char test_unchar[strlen(test_char)];
	memcpy(test_unchar,test_char,strlen(test_char));
	hash_input(test_unchar,strlen(test_char));

	hash_file.open("hash_file",std::ios::binary);
	hash_file.read(hash_file_char,65);
	hash_file.close();

	hash_hex.open("hex3.txt",std::ios::binary);
	hash_hex.read(hash_to_be,65);
	hash_hex.close();
	CHECK(memcmp(hash_file_char,hash_to_be,64)==0);
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

TEST_CASE("get_random_string","funcion test") {
	unsigned char key1[16];
	unsigned char key2[16];
	unsigned char key3[32];
	gen_random_string(key1,16);

	memcpy(key2,key1,16);
	CHECK(memcmp(key1,key2,16)==0);

	gen_random_string(key2,16);
	CHECK(memcmp(key1,key2,16)!=0);

	memcpy(key3+17,key1,16);
	CHECK(memcmp(key3+17,key1,16)==0);
	gen_random_string(key3,16);
	CHECK(memcmp(key3+17,key1,16)==0);
	CHECK(memcmp(key3,key1,16)!=0);
}

TEST_CASE("gen_key_iv","generate and save") {
	unsigned char key[16];
	unsigned char iv[16];
	gen_key_iv(key,iv);

	std::ifstream file;
	char key_file[16];
	char iv_file[16];
	file.open("key_file",std::ios::binary);
	file.read(key_file,16);
	file.close();
	file.open("iv_file",std::ios::binary);
	file.read(iv_file,16);
	file.close();

	CHECK(memcmp(key,key_file,16)==0);
	CHECK(memcmp(iv,iv_file,16)==0);
}

TEST_CASE("aes_encryption1","tests_vectors1") {
	std::ifstream input_file;
	std::ifstream cipher_text;
	std::ofstream output_file;
	input_file.open("test_aes/input_file",std::ios::binary);
	cipher_text.open("test_aes/cipher_text",std::ios::binary);
	output_file.open("test_aes/output_file",std::ios::binary);
	size_t input_len = get_infile_length(&input_file);

	size_t pom = 16;
	unsigned char input[pom+1];
	char buffer[pom+1];
	char buffer2[pom+1];
	input_file.read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	unsigned char iv[16];
	unsigned char key[16];
	load_key_iv_file(key,"test_aes/key_file",iv,"test_aes/iv_file");

	mbedtls_aes_context aes;
	set_aes_enc(&aes,key);
	aes_encryption(input,&output_file,&aes,iv,input_len);
	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();

	input_file.open("test_aes/output_file",std::ios::binary);
	input_file.read(buffer,pom);
	cipher_text.read(buffer2,pom);

	CHECK(memcmp(buffer,buffer2,pom)==0);

	input_file.close();
	cipher_text.close();
}

TEST_CASE("aes_encryption2","tests_vectors2") {
	std::ifstream input_file;
	std::ifstream cipher_text;
	std::ofstream output_file;
	input_file.open("test_aes2/input_file",std::ios::binary);
	cipher_text.open("test_aes2/cipher_text",std::ios::binary);
	output_file.open("test_aes2/output_file",std::ios::binary);
	size_t input_len = get_infile_length(&input_file);

	size_t pom = 16;
	unsigned char input[pom+1];
	char buffer[pom+1];
	char buffer2[pom+1];
	input_file.read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	unsigned char iv[16];
	unsigned char key[16];
	load_key_iv_file(key,"test_aes2/key_file",iv,"test_aes2/iv_file");

	mbedtls_aes_context aes;
	set_aes_enc(&aes,key);
	aes_encryption(input,&output_file,&aes,iv,input_len);
	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();

	input_file.open("test_aes2/output_file",std::ios::binary);
	input_file.read(buffer,pom);
	cipher_text.read(buffer2,pom);

	CHECK(memcmp(buffer,buffer2,pom)==0);

	input_file.close();
	cipher_text.close();
}

TEST_CASE("aes_encryption3","tests_vectors3") {
	std::ifstream input_file;
	std::ifstream cipher_text;
	std::ofstream output_file;
	input_file.open("test_aes3/input_file",std::ios::binary);
	cipher_text.open("test_aes3/cipher_text",std::ios::binary);
	output_file.open("test_aes3/output_file",std::ios::binary);
	size_t input_len = get_infile_length(&input_file);

	size_t pom = 16;
	unsigned char input[pom+1];
	char buffer[pom+1];
	char buffer2[pom+1];
	input_file.read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	unsigned char iv[16];
	unsigned char key[16];
	load_key_iv_file(key,"test_aes3/key_file",iv,"test_aes3/iv_file");

	mbedtls_aes_context aes;
	set_aes_enc(&aes,key);
	aes_encryption(input,&output_file,&aes,iv,input_len);
	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();

	input_file.open("test_aes3/output_file",std::ios::binary);
	input_file.read(buffer,pom);
	cipher_text.read(buffer2,pom);

	CHECK(memcmp(buffer,buffer2,pom)==0);

	input_file.close();
	cipher_text.close();
}

TEST_CASE("aes_encryption4","tests_vectors4") {
	std::ifstream input_file;
	std::ifstream cipher_text;
	std::ofstream output_file;
	input_file.open("test_aes4/input_file",std::ios::binary);
	cipher_text.open("test_aes4/cipher_text",std::ios::binary);
	output_file.open("test_aes4/output_file",std::ios::binary);
	size_t input_len = get_infile_length(&input_file);

	size_t pom = 16;
	unsigned char input[pom+1];
	char buffer[pom+1];
	char buffer2[pom+1];
	input_file.read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	unsigned char iv[16];
	unsigned char key[16];
	load_key_iv_file(key,"test_aes4/key_file",iv,"test_aes4/iv_file");

	mbedtls_aes_context aes;
	set_aes_enc(&aes,key);
	aes_encryption(input,&output_file,&aes,iv,input_len);
	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();

	input_file.open("test_aes4/output_file",std::ios::binary);
	input_file.read(buffer,pom);
	cipher_text.read(buffer2,pom);

	CHECK(memcmp(buffer,buffer2,pom)==0);

	input_file.close();
	cipher_text.close();
}


TEST_CASE("test everything","compare input and decrypted one"){
	std::ofstream input2;
	input2.open("input",std::ios::binary);
	input2 << "Tohle bude testovaci soubor, tak snad to bude vsechno ok." << std::endl;
	input2.close();

	std::ifstream input;
	std::ofstream output;

	encryption("input");

	input.open("output_file",std::ios::binary);
	output.open("decrypted",std::ios::binary);
	CHECK(aes_decryption(&input,&output));
}

