#include <iostream>
#include <fstream>
#include <string.h>

#include "mbedtls/entropy.h"        //for denerating an aes key
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/aes.h"            //for aes encryption

/**
 * void gen_aes_key(unsigned char *key)
 * @param key - 32 byte key will be generate
 */
unsigned char KEY_GLOB_ENC[32];
unsigned char IV_GLOB_ENC[16];


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
	unsigned char iv[16]= {0xb2, 0x4b, 0xf2, 0xf7, 0x7a, 0xc5, 0xec, 0x0c, 0x5e, 0x1f, 0x4d, 0xc1, 0xae, 0x46, 0x5e, 0x75};; //TODO - naplnit IV nahodnymi daty
	unsigned char input[16];
	unsigned char output[16];
	char buffer[33];
	size_t input_len = get_infile_length(input_file);
	size_t output_len = 0;

	//generating and setting the key for encryption
	 gen_aes_key(key);
	 mbedtls_aes_init(&aes);
	 if (!mbedtls_aes_setkey_enc( &aes, key, 256 )==0) {
		 std::cout << "key set NOK" << std::endl;
	 };

	std::ofstream key_file;
	key_file.open("key_file");
	std::copy(key,key+32,buffer);
	std::copy(key,key+32,KEY_GLOB_ENC);
	key_file.write(buffer,32);
	key_file.close();

	std::ofstream iv_file;
	iv_file.open("iv_file");
	std::copy(iv,iv+16,buffer);
	std::copy(iv,iv+16,IV_GLOB_ENC);
	iv_file.write(buffer,16);
	iv_file.close();


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
	 }
//TODO nejsou tu i nejake uzaviraci aes funkce...?
}

void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file) {
	mbedtls_aes_context aes;
	unsigned char key[32];
	unsigned char iv[16];
	unsigned char input[16];
	unsigned char output[16];
	char buffer[33];
	size_t input_len = get_infile_length(enc_file);
	size_t output_len = 0;
	char buffer2[17];

	std::ifstream key_file; //TODO kontrola existence souboru a nenulovosti
	key_file.open("key_file");
	key_file.get(buffer,33);
	std::copy(buffer,buffer+32,key);
	key_file.close();

	std::ifstream iv_file;
	iv_file.open("iv_file");
	iv_file.get(buffer2,17);
	std::copy(buffer2, buffer2+16,iv);
	iv_file.close();

	mbedtls_aes_init(&aes);
	if (!mbedtls_aes_setkey_dec( &aes, key, 256 )==0) {
		std::cout << "key set NOK" << std::endl;
	};
	std::cout << "\nDecryption:\n";
	while(input_len > 16) {
		enc_file->get(buffer2,17);
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
}


int main() {
	unsigned char key[32]; //TODO proc deklaruju key uz tady??

	std::ifstream in_file;
	std::ofstream out_file;
	in_file.open("input_file.txt");
	out_file.open("output_file.txt");

	std::string puvodni_input;

	aes_encryption(key,&in_file,&out_file);
	in_file.close();
	out_file.close();

	in_file.open("output_file.txt");
	out_file.open("new_input_file.txt");

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