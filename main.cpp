#include <iostream>
#include <fstream>
#include <string.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/aes.h"
//#include "mbedtls/sha512.h"


/**
 * size_t get_infile_length(std::ifstream *input_file)
 * @param input_file
 * @return the length of input_file
 */
size_t get_infile_length(std::ifstream *input_file) {
	std::streampos begin,end;
	begin = input_file->tellg();
	input_file->seekg (0, input_file->end);
	end = input_file->tellg();
	input_file->seekg(0,input_file->beg);
	return (size_t) (end - begin);  //TODOD check if is not 0
}


/**
 * void write_in_file(const unsigned char* string, size_t string_length, const char* file_name )
 * @param string - string of unsigned char which should be written in file
 * @param string_length - length of string
 * @param file_name
 * @return true is everything is OK, otherwise false
 */
bool write_in_file(const unsigned char* string, size_t string_length, const char* file_name ) {
	std::ofstream file;
	char buffer[string_length+1];
	file.open(file_name,std::ios::binary);
	if (file.is_open()) {
	std::copy(string,string+string_length,buffer);
	file.write(buffer,string_length);
	} else {
		std::cout << "Error in opennig file." << std::endl;
		return false;
	}
	file.close();
	return true;
}


/**
 * void gen_aes_key(unsigned char *key)
 * @param key - 32 byte key will be generate
 * @param length
 * @return true is everything is OK, otherwise false
 */
bool gen_aes_key(unsigned char *key, unsigned int length){
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;

	char random_string[] = "pb173 test";
	char *pers;
	pers = random_string;
	int ret;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *) pers, strlen(pers));

	if (ret != 0 ){
		printf( " failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
		return false;
	}

	ret = mbedtls_ctr_drbg_random( &ctr_drbg, key, length );
	if( ret != 0 )
	{
		printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
		return false;
	}

	return true;
}



/**
 *void aes_encryption(std::ifstream *input_file, std::ofstream *output_file)
 * @param input_file - file which will be encrypted
 * @param output_file - file in which will be saved the encrypted input_file
 */
void aes_encryption(std::ifstream *input_file, std::ofstream *output_file){
	mbedtls_aes_context aes;
	mbedtls_sha512_context sha;
	unsigned char iv[16];
	unsigned char key[32];
	size_t input_len = get_infile_length(input_file);
	if (input_len == 0) {
		std::cout << "Size of input file is 0. Encryption was ended without success.";
		return;
	}
	size_t pom = (input_len/16+1)*16;

	unsigned char input[pom+1];
	unsigned char output[pom+1];
	unsigned char output_hash[64];
	char buffer[pom+1 < 32 ? 33 : pom +1];

	(*input_file).read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	mbedtls_sha512_init(&sha);
	mbedtls_sha512(input,input_len,output_hash,0);
	write_in_file(output_hash,64,"hash_file");

	unsigned char add = (unsigned char) pom-input_len;
	for (unsigned int i = input_len; i <pom; i++) {
		input[i]=add;
	}


	if (!(gen_aes_key(iv,16) && gen_aes_key(key,32))) {
		std::cout << "Error in generating random key and iv, encryption ended without success." << std::endl;
		return;
	}

	mbedtls_aes_init(&aes);
	if (mbedtls_aes_setkey_enc( &aes, key, 256 )) {
		std::cout << "key set NOK" << std::endl;
		return;
	};


	if (!(write_in_file(key,32,"key_file") && write_in_file(iv,16,"iv_file"))) {
		std::cout << "Encryption ended without saving key and iv.";
		return;
	}

	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, pom, iv, input, output );
	std::copy(output,output+pom,buffer);
	output_file->write(buffer,pom);


	mbedtls_sha512_free(&sha);
	mbedtls_aes_free(&aes);
}


/**
 * void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file)
 * @param enc_file - encrypted file which will be decrypted
 * @param dec_file - file in which will be saved the decrypted enc_file
 */
void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file) {
	mbedtls_aes_context aes;
	mbedtls_sha512_context sha;
	unsigned char key[32];
	unsigned char iv[16];

	size_t input_len = get_infile_length(enc_file);
	unsigned char input[input_len];
	unsigned char output[input_len];

	char buffer[input_len > 32 ? input_len + 1 : 33];
	size_t output_len = input_len;
	unsigned char output_hash[64];


	std::ifstream key_file;
	key_file.open("key_file",std::ios::binary);
	if (key_file.is_open()) {
		key_file.read(buffer,32);
		std::copy(buffer,buffer+32,key);
		key_file.close();
	} else {
		std::cout << "Error in opening key_file. Decryption ended without success." << std::endl;
		return;
	}

	std::ifstream iv_file;
	iv_file.open("iv_file",std::ios::binary);
	if (iv_file.is_open()) {
		iv_file.read(buffer,16);
		std::copy(buffer, buffer+16,iv);
		iv_file.close();
	} else {
		std::cout << "Error in opening iv_file. Decryption ended without success." << std::endl;
		return;
	}

	mbedtls_aes_init(&aes);
	if (mbedtls_aes_setkey_dec( &aes, key, 256 )) {
		std::cout << "key set NOK" << std::endl;
		return;
	};

	enc_file->read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);
	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_DECRYPT, input_len, iv, input, output );


	unsigned int may_added = (unsigned int) output[input_len-1];
	bool was_added = true;
	for (unsigned int i = 1; i<= may_added; i++) {
		if (output[input_len-1]!=output[input_len-i]) {
			was_added = false;
		}
	}

	if (was_added) {
		std::copy(output,output+input_len-may_added,buffer);
		dec_file->write(buffer,input_len-may_added);
	} else {
		std::copy(output,output+input_len,buffer);
		dec_file->write(buffer,input_len);
	}


	mbedtls_sha512_init(&sha);
	mbedtls_sha512(output,output_len-may_added,output_hash,0);

	std::ifstream hash_file;
	hash_file.open("hash_file");
	if (!hash_file.is_open()) {
		std::cout << "Error in opening hash_file to check hash." << std::endl;
		return;
	}
	char hash_buffer[65];
	unsigned char hash_to_check[64];
	hash_file.read(hash_buffer,64);
	std::copy(hash_buffer,hash_buffer+64,hash_to_check);

	if(memcmp(output_hash,hash_to_check,64)) {
		std::cout << "Hash is NOK." << std::endl;
	} else {
		std::cout << "Hash is OK." << std::endl;
	}

	mbedtls_sha512_free(&sha);
	mbedtls_aes_free(&aes);
}


int main2() {

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
		out_file.open("output_file",std::ios::binary);
		if (!(in_file.is_open() && out_file.is_open())) {
			std::cout << "Some trouble with opening file." << std::endl;
			return 1;
		}
		aes_encryption(&in_file,&out_file);
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