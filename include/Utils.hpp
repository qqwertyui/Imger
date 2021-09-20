#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

namespace Utils {
    class Data {
    public:
        unsigned char *data;
        unsigned int data_size;

        Data(unsigned char *data, unsigned int data_size);
        Data();
    };

    typedef std::vector<Data *> Data_vector;

    class Chunk_manager {
    public:
        unsigned int elements;
        unsigned int total_bytes;
        Data_vector entry;

        Chunk_manager();
        ~Chunk_manager();

        void add(unsigned char *data, unsigned int datasz);
        unsigned char *to_bytearray();
    };

    std::vector<unsigned char> zlib_inflate(std::vector<unsigned char> &input);
    std::vector<unsigned char> read_file(std::string &filename, std::ifstream::openmode flags = std::ifstream::in);

    uint32_t crc32(unsigned char *data, size_t datasz);

    /* switch endianess */
    uint32_t se32(uint32_t number);
    uint16_t se16(uint16_t number);

    /* paeth predictor */
    int paeth(int left, int top, int top_left);
};


#endif