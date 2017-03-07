
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slankdev/filefd.h>
#include <slankdev/exception.h>



namespace slankdev {



template <class Tag>
class log {
        log(const log&) = delete;
        log& operator=(const log&) = delete;

    private:
        static bool inited;
        slankdev::filefd fd;

        log() {}
        ~log() {}

    public:
        static log& instance()
        {
            static log l;
            return l;
        }
        void open(const char* path)
        {
            if (inited)
                throw slankdev::exception("Inited yet");
            inited = true;
            fd.fopen(path, "a");
        }
        void open_new(const char* path)
        {
            if (inited)
                throw slankdev::exception("Inited yet");
            inited = true;
            fd.fopen(path, "w");
        }
        template<typename ... ARG>
        void write(const char* fmt, const ARG&... arg)
        {
            if (!inited)
                throw slankdev::exception("Not inited yes");

            fd.fprintf(" ");
            fd.fprintf(fmt, arg...);
            fd.fprintf("\n");
            fd.fflush();
        }
        void export_to_file(const char* output_path)
        {
            if (!inited)
                throw slankdev::exception("Not inited yes");

            fd.fflush();
            slankdev::filefd read_fd;
            slankdev::filefd write_fd;
            read_fd.fopen(fd.name.c_str(), "r");
            write_fd.fopen(output_path, "w");

            char c;
            while (read_fd.fread(&c, sizeof(c), 1)!=0) {
                write_fd.fwrite(&c, sizeof(c), 1);
            }
        }
};
template <class Tag>
bool log<Tag>::inited = false;



} /* namespace slankdev */
