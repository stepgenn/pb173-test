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
	unsigned char iv[16]; //TODO - naplnit IV nahodnymi daty
	unsigned char input[16];
	unsigned char output[16];
	char buffer[17];
	size_t input_len = get_infile_length(input_file);
	size_t output_len = 0;

	//generating and setting the key for encryption
	 gen_aes_key(key);
	 mbedtls_aes_init(&aes);
	 if (mbedtls_aes_setkey_enc( &aes, key, 256 )==0) {
		 std::cout << "key set OK" << std::endl;
	 };

	std::ofstream key_file;
	key_file.open("key_file");
	std::copy(key,key+15,buffer);
	key_file.write(buffer,16);
	key_file.close();

	std::ofstream iv_file;
	iv_file.open("iv_file");
	std::copy(iv,iv+15,buffer);
	iv_file.write(buffer,16);
	iv_file.close();

/*	 std::streampos begin,end;;
	 begin = input_file->tellg();
	 input_file->seekg (0, input_file->end);
	 end = input_file->tellg();
	 input_len = (size_t) (end - begin);
	 input_file->seekg(0,input_file->beg);
*/
	std::cout << "iv " << iv;
	 while(input_len >= 16) {
		 (*input_file).get(buffer,17); //TODO jak z toho udelat unsigned char??
		 std::copy(buffer,buffer+15,input);
		 mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output );
		 std::copy(output,output+15,buffer);
		 output_file->write(buffer,16);
		 std::cout << "input: " << input << std::endl << "output:" << output << "iv " << iv;
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
		 std::copy(output,output+15,buffer);
		 output_file->write(buffer,16);
		 std::cout << "input: " << input << std::endl << "output:" << output << "iv " << iv;
		 input_len -= 16;
		 output_len += 16;
	 }
//TODO nejsou tu i nejake uzaviraci aes funkce...?
/*		std::cout << "input: " << input << std::endl << "output: " << output << std::endl;

		for (int i = 1; i < 16; i++) {
			iv[i]= (unsigned) 2*i;
		}

	std::string hw = "helloword";
	std::copy(hw.begin(),hw.end(),input);
		std::cout << "input: " << input << std::endl << "output: " << output << std::endl;

	mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output );

		std::cout << "input: " << input << std::endl << "output: " << output << std::endl;
*/

}

void aes_decryption(unsigned char *key, std::ifstream *enc_file, std::ofstream *dec_file) {
	mbedtls_aes_context aes;
	unsigned char iv[16]; //TODO - naplnit IV nahodnymi daty
	unsigned char input[16];
	unsigned char output[16];
	char buffer[17];
	size_t input_len = get_infile_length(input_file);
	size_t output_len = 0;
}

/*int main() {
	std::cout << "Hello, World!" << std::endl;

	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
	unsigned char key[32];

	char random_string[] = "pb173 test";
	char *pers;
	pers = random_string;
	int ret;

	mbedtls_entropy_init( &entropy );
	mbedtls_ctr_drbg_init( &ctr_drbg );

	if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
	                                   (unsigned char *) pers, strlen( pers ) ) ) != 0 )
	{
		printf( " failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
		//goto exit;
	}

	if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, key, 32 ) ) != 0 )
	{
		printf( " failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
		//goto exit;
	}

	std::cout << key << std::endl;

	return 0;
}
 */

int main() {
	unsigned char key[32]; //TODO proc deklaruju key uz tady??

	std::ifstream in_file;
	std::ofstream out_file;
	in_file.open("input_file.txt");
	out_file.open("output_file.txt");



	//std::cout << "size is: " << (end-begin) << " bytes.\n";
	aes_encryption(key,&in_file,&out_file);
//	char buffer[150];
//	in_file.get(buffer,12);
//	std::cout << buffer << std::endl;


	/*gen_aes_key(key);
	std::cout << key << std::endl;
	gen_aes_key(key);
	std::cout << key << std::endl;
	*/
	in_file.close();
	out_file.close();
	return 0;
};