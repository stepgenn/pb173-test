//
// Created by sGenn on 28.02.2018.
//

#include "func.hpp"

/**
 * size_t get_infile_length(std::ifstream *input_file)
 * @param input_file
 * @return the length of input_file
 */
size_t get_infile_length(std::ifstream *input_file) {
	std::streampos begin,end;
	input_file->seekg(0,input_file->beg);
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

bool gen_and_set_aes_enc(mbedtls_aes_context *aes) {
	unsigned char key[32];

	if (!gen_aes_key(key,32)) {
		std::cout << "Error in generating random key, encryption ended without success." << std::endl;
		return false;
	}

	mbedtls_aes_init(aes);
	if (mbedtls_aes_setkey_enc(aes, key, 256 )) {
		std::cout << "Key set NOK" << std::endl;
		return false;
	};


	if (!write_in_file(key,32,"key_file")) {
		std::cout << "Encryption ended without saving key and iv.";
		return false;
	}
	return true;
}


bool aes_encryption(unsigned char* input, std::ofstream* output_file, mbedtls_aes_context *aes, unsigned char* iv, size_t input_len){
	size_t pom = (input_len/16+1)*16;

	unsigned char output[pom+1];
	char buffer[pom+1 < 32 ? 33 : pom +1];


	unsigned char add = (unsigned char) pom-input_len;
	for (unsigned int i = input_len; i <pom; i++) {
		input[i]=add;
	}

	mbedtls_aes_crypt_cbc( aes, MBEDTLS_AES_ENCRYPT, pom, iv, input, output );
	std::copy(output,output+pom,buffer);
	output_file->write(buffer,pom);
}


void hash_input(unsigned char* input, size_t input_len){
	mbedtls_sha512_context sha;
	unsigned char output_hash[64] ;
	mbedtls_sha512_init(&sha);
	mbedtls_sha512(input,input_len,output_hash,0);
	write_in_file(output_hash,64,"hash_file");
	mbedtls_sha512_free(&sha);
}


void encryption(const char *infile_name) {
	std::ifstream input_file;
	std::ofstream output_file;
	input_file.open(infile_name,std::ios::binary);
	size_t input_len = get_infile_length(&input_file);

	output_file.open("output_file",std::ios::binary);
	if (!(input_file.is_open() && output_file.is_open())) {
		std::cout << "Some trouble with opening file." << std::endl;
		return;
	}

	if (input_len == 0) {
		std::cout << "Size of input file is 0. Encryption was ended without success.";
		return;
	}

	size_t pom = (input_len/16+1)*16;
	unsigned char input[pom+1];
	char buffer[input_len];
	input_file.read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);
	hash_input(input,input_len);

	unsigned char iv[16];
	if (!(gen_aes_key(iv,16))) {
		std::cout << "Error in generating random key and iv, encryption ended without success." << std::endl;
		return;
	}
	if (!write_in_file(iv,16,"iv_file")) {
		std::cout << "Encryption ended without saving iv.";
		return;
	}

	mbedtls_aes_context aes;
	gen_and_set_aes_enc(&aes);

	aes_encryption(input,&output_file,&aes,iv,input_len);

	mbedtls_aes_free(&aes);
	input_file.close();
	output_file.close();
}



/**
 *void aes_encryption(std::ifstream *input_file, std::ofstream *output_file)
 * @param input_file - file which will be encrypted
 * @param output_file - file in which will be saved the encrypted input_file
 */
//void aes_encryption2(std::ifstream *input_file, std::ofstream *output_file){
/*	mbedtls_aes_context aes;
	unsigned char iv[16];
	size_t input_len = get_infile_length(input_file);
	if (input_len == 0) {
		std::cout << "Size of input file is 0. Encryption was ended without success.";
		return;
	}
	size_t pom = (input_len/16+1)*16;

	unsigned char input[pom+1];
	unsigned char output[pom+1];
	char buffer[pom+1 < 32 ? 33 : pom +1];

	input_file->read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input); */

/*	hash_input(input,input_len);


	unsigned char add = (unsigned char) pom-input_len;
	for (unsigned int i = input_len; i <pom; i++) {
		input[i]=add;
	}
/*	if (!(gen_aes_key(iv,16) && gen_aes_key(key,32))) {
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

	if (!(gen_aes_key(iv,16))) {
		std::cout << "Error in generating random key and iv, encryption ended without success." << std::endl;
		return;
	}
	if (!write_in_file(iv,16,"iv_file")) {
		std::cout << "Encryption ended without saving iv.";
		return;
	}*/

//	gen_and_set_aes_enc(&aes);

//	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, pom, iv, input, output );
//	std::copy(output,output+pom,buffer);
//	output_file->write(buffer,pom);


//	mbedtls_sha512_free(&sha);
//	mbedtls_aes_free(&aes);
//}


/**
 * void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file)
 * @param enc_file - encrypted file which will be decrypted
 * @param dec_file - file in which will be saved the decrypted enc_file
 */
bool aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file) {
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
		return false;
	}

	std::ifstream iv_file;
	iv_file.open("iv_file",std::ios::binary);
	if (iv_file.is_open()) {
		iv_file.read(buffer,16);
		std::copy(buffer, buffer+16,iv);
		iv_file.close();
	} else {
		std::cout << "Error in opening iv_file. Decryption ended without success." << std::endl;
		return false;
	}

	mbedtls_aes_init(&aes);
	if (mbedtls_aes_setkey_dec( &aes, key, 256 )) {
		std::cout << "key set NOK" << std::endl;
		return false;
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
		return false;
	}
	char hash_buffer[65];
	unsigned char hash_to_check[64];
	hash_file.read(hash_buffer,64);
	std::copy(hash_buffer,hash_buffer+64,hash_to_check);

	if(memcmp(output_hash,hash_to_check,64)) {
		std::cout << "Hash is NOK." << std::endl;
		mbedtls_sha512_free(&sha);
		mbedtls_aes_free(&aes);
		return false;
	} else {
		std::cout << "Hash is OK." << std::endl;
		mbedtls_sha512_free(&sha);
		mbedtls_aes_free(&aes);
		return true;
	}


}
