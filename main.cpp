#include "includes.hpp"
#include "generator.hpp"

int main()
{
    std::string n = "french-names-1";

    Generator gen(3);
    gen.loadFromList("list/" + n + ".txt");

    std::cout << "Saving..." << std::endl << std::endl;

    gen.saveToFile(n);

    std::cout << "Generating..." << std::endl << std::endl;

    //std::cout << "\"" << gen.previous("test", 1, 3) << "\"" << std::endl;

    for(int i = 0; i < 10; i++)
    {
        //std::cout << randomFloat() << std::endl;
        //std::string name = gen.randomString((rand()%5) + 5);

        std::string name = gen.randomString();
        std::cout << name << std::endl;
    }

    return 0;
}