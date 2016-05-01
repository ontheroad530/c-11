#include <iostream>

#include "variant.h"

using namespace std;

int main()
{
    typedef Variant<int, double, std::string> cv;

    cv v = 10;
    cout << typeid( cv::IndexType<0>).name() << endl;
    cout << typeid( cv::IndexType<1>).name() << endl;
    cout << typeid( cv::IndexType<2>).name() << endl;
    int i = v.get<int>();
    cout << i << endl;

    v = cv(std::string("hello") );
    std::cout << v.get<std::string>() << std::endl;


    return 0;
}

