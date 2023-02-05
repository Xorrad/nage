#include "includes.hpp"
#include "generator.hpp"

int main()
{
    Generator gen;

    gen.loadFromFile("data/german-cities");

    std::cout << "Generating..." << std::endl << std::endl;

    for(int i = 0; i < 10; i++)
    {
        std::string name = gen.randomString();
        std::cout << name << std::endl;
    }

    return 0;
}