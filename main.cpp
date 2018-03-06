#include <iostream>
#include <fstream>
#include <cstring>


#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/aes.h"
//#include "mbedtls/sha512.h"
#include "func.hpp"


int main() {
	std::ifstream hash_file;
	std::ifstream hash_hex;
	char hash_file_char[65];
	char hash_to_be[65];

//	char test_abc_char[] = "";
	unsigned char test_empty[]={};
//	memcpy(test_abc,test_abc_char,strlen(test_abc_char));
	hash_input(test_empty,0);

	hash_file.open("hash_file",std::ios::binary);
	hash_file.read(hash_file_char,65);
	hash_file.close();

	hash_hex.open("hex2.txt",std::ios::binary);
	hash_hex.read(hash_to_be,65);
	hash_hex.close();
	if((memcmp(hash_file_char,hash_to_be,64)==0)) {
		std::cout <<"yes\n" ;
	}


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