#include <iostream>
#include <fstream>
#include <cstring>


#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/aes.h"
//#include "mbedtls/sha512.h"
#include "func.hpp"


int main() {
/*	std::ifstream input_file;
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
	hash_input(input,input_len);

	unsigned char iv[16];
	unsigned char key[32];
	load_key_iv_file(key,"test_aes/key_file",iv,"test_aes/iv_file");
	memcpy(key,buffer2,32);

	mbedtls_aes_context aes;
	set_aes_enc(&aes,key);
	aes_encryption(input,&output_file,&aes,iv,input_len);
	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();

	input_file.open("test_aes/output_file",std::ios::binary);
	input_file.read(buffer,pom);
	cipher_text.read(buffer2,pom);
	if(memcmp(buffer,buffer2,pom)!=0){
		std::cout << "NOK NOK NOK :( \n";
	} else {
		std::cout << "OK OK OK :) \n";
	}

	input_file.close();
	cipher_text.close(); */


	std::cout << "What do you want to do?" << std::endl;
	std::cout << "1) Encrypt and hash supplied file, write into out file.\n";
	std::cout << "2) Decrypt and verify hash.\n";
	std::cout << "Write a number: ";
	unsigned int opt = 0;
	std::cin >> opt;

	std::ifstream in_file;
	std::ofstream out_file;

	if (opt==1) {
		std::cout << "Write the name of file (input for encryption): " << std::endl;
		std::string file_name;
		std::getline(std::cin,file_name);
		std::getline(std::cin,file_name);
		in_file.open(file_name.c_str(),std::ios::binary);
/*		out_file.open("output_file",std::ios::binary);
		if (!(in_file.is_open() && out_file.is_open())) {
			std::cout << "Some trouble with opening file." << std::endl;
			return 1;
		}*/
//		aes_encryption(&in_file,&out_file);
		encryption(file_name.c_str());
		in_file.close();
		out_file.close();
		std::cout << "\nOutput files: key_file, iv_file, hash_file, output_file" << std::endl;
	} else if (opt==2) {
		std::cout << "Value for key, initialization vector and hash to be chacked will be taken from files key_file, "
			"iv_file and hash_file.\nWrite the name of encrypted file: " << std::endl;
		std::string file_name;
		std::getline(std::cin,file_name);
		std::getline(std::cin,file_name);
		in_file.open(file_name.c_str(),std::ios::binary);
		out_file.open("decrypted_file",std::ios::binary);
		if (!(in_file.is_open() && out_file.is_open())) {
			std::cout << "Some trouble with opening file." << std::endl;
			return 2;
		}
		aes_decryption(&in_file,&out_file);
		in_file.close();
		out_file.close();
	} else {
		std::cout << "You didn't write any of option above." <<std::endl;
	}

	return 0;
};