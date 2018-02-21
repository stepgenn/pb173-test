#include <iostream>
#include <string.h>
#include "mbedtls/aes.h"            //for aes encryption
#include "mbedtls/entropy.h"        //for denerating an aes key
#include "mbedtls/ctr_drbg.h"

void gen_aes_key(){
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
	unsigned char key[32];

	char *pers = "pb173 test";
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


void aes_encryption(){
	mbedtls_aes_context aes;
	unsigned char key[32];
	unsigned char iv[16];
	unsigned char input[128];
	unsigned char output[128];
	size_t input_len = 40;
	size_t output_len = 0;
}

int main() {
	std::cout << "Hello, World!" << std::endl;
	return 0;
}