#ifndef GENERICEXCEPTION_HPP
#define GENERICEXCEPTION_HPP

namespace Code {
    enum {
        OK = 0,
        FILE_ERROR = 1,
        UNSUPPORTED_FORMAT = 2,
        UNRECOGNIZED_FORMAT = 3,
        UNSUPPORTED_COLOR_DEPTH = 4,
        UNSUPPORTED_COMPRESSION_METHOD = 5,
        UNSUPPORTED_COLOR_TYPE = 6,
        INVALID_ARGS = 7,
        INVALID_CHUNK_TYPE = 8,
        SDL_INIT_ERROR = 9,
        DECOMPRESSION_ERROR = 10,
        DISPLAY_INFO_ERROR = 11,
        CHUNK_LOAD_ERROR = 12,
        INVALID_PLTE_LENGTH = 13,
        CRITICAL_CHUNKS_NOT_FOUND = 14,
        NOT_ICO = 15,
        PLTE_NOT_FOUND = 16,
        INVALID_PIXELFORMAT = 17
    };
};

class GenericException {
public:
    GenericException(const char *msg, int code);
    const char* what();
    int get_code();

private:
    const char *msg;
    int code;
};

#endif