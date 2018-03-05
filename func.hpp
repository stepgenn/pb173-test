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
bool gen_aes_key(unsigned char *key, unsigned int length);

/**
 *void aes_encryption(std::ifstream *input_file, std::ofstream *output_file)
 * @param input_file - file which will be encrypted
 * @param output_file - file in which will be saved the encrypted input_file
 */
void aes_encryption(std::ifstream *input_file, std::ofstream *output_file);

/**
 * void aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file)
 * @param enc_file - encrypted file which will be decrypted
 * @param dec_file - file in which will be saved the decrypted enc_file
 */
bool aes_decryption(std::ifstream *enc_file, std::ofstream *dec_file);

#endif //PB173_TEST_FUNC_HPP
