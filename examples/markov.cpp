#include "../nage.hpp"
#include <filesystem>

class CityNameGenerator : public nage::Generator {
public:
    CityNameGenerator() {
        m_MarkovGenerator = nage::Make<nage::MarkovChainGenerator>(3);
        m_MarkovGenerator->LoadCacheOrCompute("data/markov-cities.bin", "list/german-cities.txt");
    }

    virtual std::string Generate() override {
        return m_MarkovGenerator->Generate();
    }
private:
    std::unique_ptr<nage::MarkovChainGenerator> m_MarkovGenerator;
};

int main() {
    srand(time(NULL));

    // Initialize nage (create context, prepare handler)
    nage::Init();

    std::unique_ptr<CityNameGenerator> generator = nage::Make<CityNameGenerator>();

    // Add filters and modifiers to a generator
    nage::PreparedGenerator<CityNameGenerator> prepared = generator->Prepare<CityNameGenerator>()
        .Filter([&](std::string name) {
            // return nage::string::StrLength(name) == 8;
            return true;
        })
        .Edit([&](std::string name) {
            name[0] = toupper(name[0]);
            return name;
        })
        .Generate([&](CityNameGenerator* generator){
            return generator->Generate();
        });
    
    // Generate a few names
    for(int i = 0; i < 10; i++)
        printf("%s\n", prepared.Get().c_str());

    nage::Free();
    return 0;
}