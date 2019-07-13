
#include "veb.h"

#include <iostream>
#include <random>

using T = unsigned long long;
VEBtree<T, 64> tree;

#define i(val) tree.insert(val)
#define p(val) tree.predecessor(val)

T m_random() {
    static std::random_device rd; 
    static std::mt19937_64 eng(rd()); 
    static std::uniform_int_distribution<T> distr;
    return distr(eng);
}

int main()
{
    for (size_t j = 0; j < (1 << 20); ++j)
	i(m_random());
    for (size_t i = 0; i < (1 << 20); ++i)
	p(m_random());
}
