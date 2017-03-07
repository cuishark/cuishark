

#pragma once

#include <stdio.h>
#include <string>

namespace slankdev {

class filefd {
    private:
        FILE* fp;
        std::string name;

    public:
        filefd() : fp(nullptr) {}
        ~filefd()
        {
            fclose();
        }
        FILE* getfp() { return fp; }


        void fopen(const char* path, const char* mode)
        {
            if (fp)
                fclose();

            fp = ::fopen(path, mode);
            if (!fp) {
                perror("fopen");
                exit(-1);
            }
            name = path;
        }
        void fclose()
        {
            if (fp != nullptr) {
                ::fclose(fp);
                fp = nullptr;
                name = "";
            }
        }
        void fwrite(const void* ptr, size_t size, size_t nmemb)
        {
            size_t res = ::fwrite(ptr, size, nmemb, fp);
            if (res != nmemb) {
                perror("fwrite");
                exit(-1);
            }
        }
        char* fgets(char* s, int size)
        {
           char* res = ::fgets(s, size, fp);
           return res;
        }
        size_t fread(void* ptr, size_t size, size_t nmemb)
        {
            size_t res = ::fread(ptr, size, nmemb, fp);
            if (res != nmemb) {
                if (errno == 0)
                    return res;
                else
                    perror("fread");
            }
            return res;
        }
        void fflush()
        {
            int res = ::fflush(fp);
            if (res == EOF) {
                perror("fflush");
                exit(-1);
            }
        }
        template<typename... ARG>
        int fprintf(const char* fmt, const ARG&... arg)
        {
            return ::fprintf(fp, fmt, arg...);
        }
};


} /* namespace slankdev */


