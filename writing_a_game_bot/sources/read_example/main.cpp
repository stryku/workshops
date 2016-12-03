#include <iostream>

int main()
{
    static int static_int = 0;
    int normal_int = 0;

    while (true)
    {
        std::cout << "Static: ";
        std::cin >> static_int;

        std::cout << "Normal: ";
        std::cin >> normal_int;

        std::cout << "\n";
    }

    return 0;
}