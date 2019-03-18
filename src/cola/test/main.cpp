#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include <sstream>

#include "utility.hpp"
#include "cola.hpp"

void test_flag();
void test_args();
void test_vector_args();
void test_vector_initialize();
void test_rest_arg();
void test_undefined_options();
void usage_example(int, char*[]);


int main(int argc, char* argv[])
{
    test_flag();
    test_args();
    test_vector_args();
    test_vector_initialize();
    test_rest_arg();
    test_undefined_options();
    std::cout << "\033[32m[[[ ALL TEST PASSED! ]]]\033[m\n" << std::endl;

    usage_example(argc, argv);

    return 0;
}


