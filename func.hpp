//
// Created by sGenn on 28.02.2018.
//

#ifndef PB173_TEST_FUNC_HPP
#define PB173_TEST_FUNC_HPP
#include <iostream>
#include <fstream>
#include <cstring>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/aes.h"
//#include "mbedtls/sha512.h"


/**
 * hash_input - the function takes a pointer to string of unsigned char and int length and hash it and save the hash to a file
 * @param input - input to be hash
 * @param input_len - length of input
 */
void hash_input(unsigned char* input, size_t input_len);

/**
 * generate and save into file iv and key
 * @param key
 * @param iv
 * @return
 */
bool gen_key_iv(unsigned char *key, unsigned char *iv);
/**
 * gen_and_set_aes_enc(mbedtls_aes_context *aes) - generate random key, save it to a file and set aes with key
 * @param aes - context which should be set
 * @return
 */
bool set_aes_enc(mbedtls_aes_context *aes, unsigned char* key);

/**
 * size_t get_infile_length(std::ifstream *input_file)
 * @param input_file
 * @return the length of input_file
 */
size_t get_infile_length(std::ifstream *input_file);

/**
 * void write_in_file(const unsigned char* string, size_t string_length, const char* file_name )
 * @param string - string of unsigned char which should be written in file
 * @param string_length - length of string
 * @param file_name
 * @return true is everything is OK, otherwise false
 */
bool write_in_file(const unsigned char* string, size_t string_length, const char* file_name );

/**
 * void gen_aes_key(unsigned char *key)
 * @param key - 32 byte key will be generate
 * @param length
 * @return true is everything is OK, otherwise false
 */
bool gen_random_string(unsigned char *key, unsigned int length);


/**
 * bool aes_encryption(unsigned char* input, std::ofstream* output_file, mbedtls_aes_context *aes, unsigned char* iv, size_t input_len)
 * - function for encryption with aes
 * @param input - string of unsigned char
 * @param output_file - file to write outptu in
 * @param aes - mbeddtls context
 * @param iv - initialization vector
 * @param input_len - length of input
 * @return
 */
void aes_encryption(unsigned char* input, std::ofstream* output_file, mbedtls_aes_context *aes, unsigned char* iv, size_t input_len);

/**
 * encrypt file with name infile_name
 * @param infile_name
 */
void encryption(const char *infile_name);

/**
 * void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file)
 * @param enc_file - encrypted file which will be decrypted
 * @param dec_file - file in which will be saved the decrypted enc_file
 */
bool aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file);

#endif //PB173_TEST_FUNC_HPP
