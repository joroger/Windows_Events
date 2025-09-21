#include <iostream>

void test(int ptr) {
    std::cout << "Pointer in test function: " << &ptr << std::endl;
}

int main() {
    int x = 10;
    std::cout << "Pointer in main function: " << &x << std::endl;
    test(x);
    return 0;
}