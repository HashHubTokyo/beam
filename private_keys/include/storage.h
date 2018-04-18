#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <stdio.h>
#include <fstream>
#include <string.h>
#include "common.h"

// Prototype of UTXO
struct UTXO : beam::Output {

    int id;
    std::string info;

    UTXO() = default;
    UTXO(int id_, const char* info_) : id(id_), info(info_) {}

    // Encrypt UTXO and write it to filestream
    void write(std::ofstream &os, char* key);

    // Write UTXO to filestream
    void write(std::ofstream &os);
};

constexpr size_t SIZEUTXO = sizeof(UTXO);


// Create filestream for writing
std::ofstream create_out_filestream(const char* filename);

// Encryption/decryption by key
void crypto_by_key(char* input, char* output, const char* key, size_t N);

// Encode data by some key
template <class T = UTXO>
char* encode(T* data_, const char* key) {

    size_t size_data = sizeof(T);

    char* encoded = new char[size_data];

    char* data = reinterpret_cast<char*>(data_);

    crypto_by_key(data, encoded, key, size_data);

    return encoded;
}

// Decode data by some key
void decode(char* encoded, size_t data_size, const char* key);

// Recover from binary file
template<class T>
T* recover(std::ifstream &is, size_t offset, const char* key = nullptr) {

    size_t size_ = sizeof(T);

    char* buf = new char[size_];

    is.seekg(offset);
    is.read(buf, size_);
    is.seekg(0);

    if (key) decode(buf, size_, key);

    T* pu = reinterpret_cast<T*>(buf);

}

// For test encryption/decryption
char* create_some_secret_key();


#endif // STORAGE_H_INCLUDED
