#include "../nage.hpp"

int main() {
    srand(time(NULL));

    // Initialize nage (create context, prepare handler)
    nage::Init();

    std::unique_ptr<nage::FaceGenerator> generator = nage::Make<nage::FaceGenerator>("data/faces/male.txt");

    // Generate faces
    for(int i = 0; i < 10; i++)
        printf("%s\n\n", generator->Generate().c_str());

    nage::Free();
    return 0;
}