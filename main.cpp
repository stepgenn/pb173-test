#include <iostream>
#include <fstream>
#include <string.h>

#include "mbedtls/entropy.h"        //for generating an aes key
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/aes.h"            //for aes encryption
#include "mbedtls/sha512.h"

/**
 * void gen_aes_key(unsigned char *key)
 * @param key - 32 byte key will be generate
 */
size_t get_infile_length(std::ifstream *input_file) {
	std::streampos begin,end;;
	begin = input_file->tellg();
	input_file->seekg (0, input_file->end);
	end = input_file->tellg();
	input_file->seekg(0,input_file->beg);
	return (size_t) (end - begin);
}


void gen_aes_key(unsigned char *key){
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
		// goto exit;
		//TODO poresit goto exity
	}

	ret = mbedtls_ctr_drbg_random( &ctr_drbg, key, 32 );
	if( ret != 0 )
	{
		printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
		//goto exit;
		//TODO poresit goto exity
	}

}




void aes_encryption(unsigned char *key, std::ifstream *input_file, std::ofstream *output_file){
	mbedtls_aes_context aes;
	mbedtls_sha512_context sha;
	unsigned char iv[16]= {0xb2, 0x4b, 0xf2, 0xf7, 0x7a, 0xc5, 0xec, 0x0c, 0x5e, 0x1f, 0x4d, 0xc1, 0xae, 0x46, 0x5e, 0x75};; //TODO - naplnit IV nahodnymi daty
/*	unsigned char input[16];
	unsigned char output[16];
	char buffer[33];
 */
	size_t input_len = get_infile_length(input_file);
//	size_t output_len = 0;
	int pom = (input_len/16+1)*16;
	unsigned char input[pom+1];
	unsigned char output[pom+1];
	unsigned char output_hash[64];
	char buffer[pom+1];


	(*input_file).read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);

	mbedtls_sha512_init(&sha);
	mbedtls_sha512(input,input_len,output_hash,0);

	std::ofstream hash_file;
	hash_file.open("hash_file",std::ios::binary);
	std::copy(output_hash,output_hash+64,buffer);
	hash_file.write(buffer,64);
	hash_file.close();


	unsigned char add = (unsigned char) pom-input_len;
	for (unsigned int i = input_len; i <pom; i++) {
		input[i]=add;
	}

	//generating and setting the key for encryption
	 gen_aes_key(key);
	 mbedtls_aes_init(&aes);
	 if (!mbedtls_aes_setkey_enc( &aes, key, 256 )==0) {
		 std::cout << "key set NOK" << std::endl;
	 };

	std::ofstream key_file;
	key_file.open("key_file",std::ios::binary);
	std::copy(key,key+32,buffer);
	key_file.write(buffer,32);
	key_file.close();

	std::ofstream iv_file;
	iv_file.open("iv_file",std::ios::binary);
	std::copy(iv,iv+16,buffer);
	iv_file.write(buffer,16);
	iv_file.close();

	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, pom, iv, input, output );
	std::copy(output,output+pom,buffer);
	output_file->write(buffer,pom);
/*
	 while(input_len >= 16) {
		 (*input_file).get(buffer,17); //TODO jak z toho udelat unsigned char??
		 std::copy(buffer,buffer+16,input);
		 mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output );
		 std::copy(output,output+16,buffer);
		 output_file->write(buffer,16);
		 input_len -= 16;
		 output_len += 16;
	 }

	 if (input_len != 0) {
		 (*input_file).get(buffer,input_len+1); //TODO jak z toho udelat unsigned char??
		 std::copy(buffer,buffer+input_len,input);
		 unsigned char add = (unsigned char) 16-input_len;
		 for (unsigned int i = input_len; i <16; i++) {
			 input[i]=add;
		 }
		 mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output );
		 std::copy(output,output+16,buffer);
		 output_file->write(buffer,16);
		 input_len -= 16;
		 output_len += 16;
	 }*/
//TODO nejsou tu i nejake uzaviraci aes funkce...?
	mbedtls_sha512_free(&sha);
	mbedtls_aes_free(&aes);
}

void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file) {
	mbedtls_aes_context aes;
	mbedtls_sha512_context sha;
	unsigned char key[32];
	unsigned char iv[16];
/*	unsigned char input[16];
	unsigned char output[16];
	char buffer[33];
*/
	size_t input_len = get_infile_length(enc_file);
	unsigned char input[input_len];
	unsigned char output[input_len];
	char buffer[input_len+1];
	size_t output_len = input_len;
	unsigned char output_hash[64];
//	char buffer2[17];

	std::ifstream key_file; //TODO kontrola existence souboru a nenulovosti
	key_file.open("key_file",std::ios::binary);
	key_file.read(buffer,32);
	std::copy(buffer,buffer+32,key);
	key_file.close();

	std::ifstream iv_file;
	iv_file.open("iv_file",std::ios::binary);
	iv_file.read(buffer,16);
	std::copy(buffer, buffer+16,iv);
	iv_file.close();

	mbedtls_aes_init(&aes);
	if (!mbedtls_aes_setkey_dec( &aes, key, 256 )==0) {
		std::cout << "key set NOK" << std::endl;
	};
	std::cout << "\nDecryption:\n";
	enc_file->read(buffer,input_len);
	std::copy(buffer,buffer+input_len,input);
	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_DECRYPT, input_len, iv, input, output );
	std::copy(output,output+input_len,buffer);
	dec_file->write(buffer,input_len);

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

/*
	while(input_len > 16) {
		enc_file->get(buffer,17);
		std::copy(buffer2,buffer2+16,input);
		mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_DECRYPT, 16, iv, input, output );
		std::copy(output,output+16,buffer2);
		dec_file->write(buffer2,16);
		std::cout << "output:" << buffer2 << std::endl;
		input_len -= 16;
		output_len += 16;
	}

	if (input_len==16) {
		enc_file->get(buffer2,17);
		std::copy(buffer2,buffer2+16,input);
		mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_DECRYPT, 16, iv, input, output );
		unsigned int may_added = (unsigned int) output[15];
		bool was_added = true;
		for (unsigned int i = 1; i<= may_added; i++) {
			if (output[15]!=output[16-i]) {
				was_added = false;
			}
		}
		if (was_added) {
			std::copy(output,output+16-may_added,buffer2);
			dec_file->write(buffer2,16-may_added);
		} else {
			std::copy(output,output+16,buffer2);
			dec_file->write(buffer2,16);
		}

		std::cout << "output:" << buffer2 << std::endl;
	}
 */
	mbedtls_sha512_init(&sha);
	mbedtls_sha512(output,output_len-may_added,output_hash,0);  //TODO dodelat hashovani, kontrolu hashe, jestli jsou stejne, promennou pro ulozeni hashe...

	std::ifstream hash_file;
	hash_file.open("hash_file");
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


int main() {
	unsigned char key[32]; //TODO proc deklaruju key uz tady??

	std::ifstream in_file;
	std::ofstream out_file;
	in_file.open("input_file.txt",std::ios::binary);
	out_file.open("output_file.txt",std::ios::binary);

	std::string puvodni_input;

	aes_encryption(key,&in_file,&out_file);
	in_file.close();
	out_file.close();

	in_file.open("output_file.txt",std::ios::binary);
	out_file.open("new_input_file.txt",std::ios::binary);

	aes_decryption(&in_file,&out_file);


	return 0;
};


/*
//je to vpohode verze, jede to...
int main(){
	mbedtls_aes_context aes;
	mbedtls_aes_context aes2;
	unsigned char iv[16] = {0xb2, 0x4b, 0xf2, 0xf7, 0x7a, 0xc5, 0xec, 0x0c, 0x5e, 0x1f, 0x4d, 0xc1, 0xae, 0x46, 0x5e, 0x75}; //TODO - naplnit IV nahodnymi daty
	unsigned char iv2[16] = {0xb2, 0x4b, 0xf2, 0xf7, 0x7a, 0xc5, 0xec, 0x0c, 0x5e, 0x1f, 0x4d, 0xc1, 0xae, 0x46, 0x5e, 0x75}; //TODO - naplnit IV nahodnymi daty
	unsigned char key[32];
	unsigned char input[128];
	unsigned char output[128];
	unsigned char output2[128];
	char buffer[22];
	char input_text[] = "Zkusebni text pro sifrovani a desifrovani.";
	std::copy(input_text,input_text+20,input);


	gen_aes_key(key);
	mbedtls_aes_init(&aes);
	mbedtls_aes_init(&aes2);
	if (mbedtls_aes_setkey_enc( &aes, key, 256 )==0) {
//		 std::cout << "key set OK" << std::endl;
	};
	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 128, iv, input, output );
	mbedtls_aes_setkey_dec(&aes2,key,256);
	mbedtls_aes_crypt_cbc( &aes2, MBEDTLS_AES_DECRYPT, 128, iv2, output, output2);

	std::copy(output2,output2+20,buffer);
	std::cout << "input: " << input << std::endl << "output2: " << buffer << std::endl;
	if(!memcmp(input,output2,16)){
		std::cout << "je to oka.";
	}

	return 0;
}
 */