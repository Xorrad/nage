#include "../nage.hpp"

int main() {
    srand(time(NULL));

    // Initialize nage (create context, prepare handler)
    nage::Init();

    std::unique_ptr<nage::TemplateGenerator> generator = nage::Make<nage::TemplateGenerator>("data/templates/rinkworks.txt");

    // Generate a few names
    for(int i = 0; i < 10; i++)
        printf("%s\n", generator->Generate("BVCs").c_str());

    nage::Free();
    return 0;
}