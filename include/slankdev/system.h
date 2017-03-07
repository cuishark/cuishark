

#pragma once

#include <stdio.h>
#include <stdint.h>


namespace slankdev {

uint64_t rdtsc()
{
    unsigned int eax, edx;
    __asm__ volatile("rdtsc" : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}

void delay_clk(size_t num_clks)
{
    uint64_t begin = rdtsc();
    while (rdtsc() - begin < num_clks) ;
}


} /* namespace slankdev */
