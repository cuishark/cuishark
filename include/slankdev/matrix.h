
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sstream>
#include <ostream>
#include <exception>
#include <string>

namespace slankdev {


/*
 * Sample
 *
 * int main()
 * {
 *     Matrix m1(2, 3);
 *     m1.set(0, 0, 1);  m1.set(1, 0, 2);
 *     m1.set(0, 1, 4);  m1.set(1, 1, 5);
 *     m1.set(0, 2, 7);  m1.set(1, 2, 8);
 *     m1.print();
 *
 *     printf("\n");
 *
 *     Matrix m2(3, 2);
 *     m2.set(0, 0, 1);  m2.set(1, 0, 2);  m2.set(2, 0, 3);
 *     m2.set(0, 1, 4);  m2.set(1, 1, 5);  m2.set(2, 1, 6);
 *     m2.print();
 *
 *     printf("\n");
 *
 *     Matrix m3 = m1 * m2;
 *     m3.print();
 * }
 *
 */



class matrix_exception : public std::exception {
    private:
        std::string str;
    public:
        explicit matrix_exception(const char* s) noexcept {
            str = s;
        }

        template<class T>
        exception& operator<<(const T& t) noexcept {
            std::ostringstream os;
            os << " " << t ;
            str += os.str();
            return *this;
        }
        const char* what() const noexcept {
            return str.c_str();
        }
};



class Matrix {
    int** p;
    size_t x_size;
    size_t y_size;
public:
    Matrix(size_t x, size_t y) : x_size(x), y_size(y)
    {
        p = (int**)malloc(sizeof(int*) * y);
        for (size_t i=0; i<y; i++) {
            p[i] = (int*)malloc(sizeof(int) * x);
        }
    }
    Matrix(const Matrix& rhs) : Matrix(rhs.x_size, rhs.y_size)
    {
        for (size_t i=0; i<x_size; i++) {
            for (size_t j=0; j<y_size; j++) {
                p[j][i] = rhs.p[j][i];
            }
        }
    }
    ~Matrix()
    {
        free(p);
    }
    Matrix operator+(const Matrix& rhs)
    {
        if (x_size != rhs.x_size || y_size != rhs.y_size)
            throw matrix_exception("operator+");

        Matrix m(x_size, y_size);

        try {
            for (size_t i=0; i<x_size; i++) {
                for (size_t j=0; j<y_size; j++) {
                    m.p[j][i] = rhs.p[j][i] + p[j][i];
                }
            }
            return m;
        } catch (matrix_exception& e) {
            std::string errstr = "operator+: ";
            errstr += e.what();
            throw matrix_exception(errstr.c_str());
        }
    }
    Matrix operator-(const Matrix& rhs)
    {
        if (x_size != rhs.x_size || y_size != rhs.y_size)
            throw matrix_exception("operator-");


        Matrix m(x_size, y_size);

        try {
            for (size_t i=0; i<x_size; i++) {
                for (size_t j=0; j<y_size; j++) {
                    m.p[j][i] = rhs.p[j][i] - p[j][i];
                }
            }
            return m;
        } catch (matrix_exception& e) {
            std::string errstr = "operator-: ";
            errstr += e.what();
            throw matrix_exception(errstr.c_str());
        }
    }
    Matrix operator*(const Matrix& rhs)
    {
        if (x_size != rhs.y_size || y_size != rhs.x_size)
            throw matrix_exception("operator*");

        try {
            Matrix m(rhs.x_size, y_size);
            for (size_t x=0; x<rhs.x_size; x++) {
                for (size_t y=0; y<y_size; y++) {
                    /* calc p[x, y]*/

                    int sum = 0;
                    for (size_t i=0; i<x_size; i++) {
                        sum += get(i, y) * rhs.get(x, i);
                    }
                    m.set(x, y, sum);
                }
            }
            return m;
        } catch (matrix_exception& e) {
            std::string errstr = "operator*: ";
            errstr += e.what();
            throw matrix_exception(errstr.c_str());
        }
    }
    Matrix& operator=(const Matrix& rhs)
    {
        try {
            for (size_t i=0; i<x_size; i++) {
                for (size_t j=0; j<y_size; j++) {
                    p[j][i] = rhs.p[j][i];
                }
            }
            return *this;
        } catch (matrix_exception& e) {
            std::string errstr = "operator=: ";
            errstr += e.what();
            throw matrix_exception(errstr.c_str());
        }
    }
    void print()
    {
        printf("Matrix(%zd x %zd)\n", x_size, y_size);
        for (size_t i=0; i<y_size; i++) {
            for (size_t j=0; j<x_size; j++) {
                printf("%3d ", get(j, i));
            }
            printf("\n");
        }
    }
    void set(size_t x, size_t y, int val)
    {
        if (x_size <= x || y_size <= y) {
            char errstr[256];
            sprintf(errstr, "set(%2zd, %2zd, %d)", x,y,val);
            throw matrix_exception(errstr);
        }
        p[y][x] = val;
    }
    int get(size_t x, size_t y) const
    {
        if (x_size <= x || y_size <= y) {
            char errstr[256];
            sprintf(errstr, "get(%2zd, %2zd)", x,y);
            throw matrix_exception(errstr);
        }
        return p[y][x];
    }
};



} /* namespace slankdev */
