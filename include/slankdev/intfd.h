
#pragma once

#include <slankdev/exception.h>

#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>



namespace slankdev {



class safe_intfd {
    public:
        bool noclose_in_destruct;
    protected:
        int fd;
    public:
        safe_intfd() : fd(-1), noclose_in_destruct(false) {}
        ~safe_intfd()
        {
            if (!noclose_in_destruct)
                close();
        }
        void open(const char* path, int flags)
        {
            fd = ::open(path, flags);
            if (fd < 0) {
                perror("open");
                throw slankdev::exception("open");
            }
        }
        void open(const char* path, int flags, mode_t mode)
        {
            fd = ::open(path, flags, mode);
            if (fd < 0) {
                perror("open");
                throw slankdev::exception("open");
            }
        }
        void close()
        {
            if (fd >= 0)
                ::close(fd);
        }
        int fcntl(int cmd)
        {
            int res = ::fcntl(fd, cmd);
            if (res < 0) {
                perror("fcntl");
                throw slankdev::exception("fcntl");
            }
            return res;
        }
        void fcntl(int cmd, long arg)
        {
            int res = ::fcntl(fd, cmd, arg);
            if (res < 0) {
                perror("fcntl");
                throw slankdev::exception("fcntl");
            }
        }
        void ioctl(unsigned long l, void* arg)
        {
            int res = ::ioctl(fd, l, arg);
            if (res < 0) {
                perror("ioctl");
                throw slankdev::exception("ioctl");
            }
        }
        void write(const void* buffer, size_t bufferlen)
        {
            ssize_t res = ::write(fd, buffer, bufferlen);
            if (static_cast<size_t>(res) != bufferlen) {
                if (res < 0) {
                    perror("write");
                    throw slankdev::exception("write");
                } else {
                    fprintf(stderr, "write could not send all.\n");
                }
            }
        }
        size_t read(void* buffer, size_t bufferlen)
        {
            ssize_t res = ::read(fd, buffer, bufferlen);
            if (res < 0) {
                perror("read");
                throw slankdev::exception("read");
            } else if (res == EINTR) {
                return read(buffer, bufferlen);
            }
            return static_cast<size_t>(res);
        }
    public:
        int get_fd()
        {
            return fd;
        }
        void set_fd(int f)
        {
            if (f >= 0)
                close();
            fd = f;
        }
};





} /* namespace slankdev */
