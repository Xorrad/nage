#include "../nage.hpp"
#include <filesystem>

class CityNameGenerator : public nage::Generator {
public:
    CityNameGenerator() {
        m_MarkovGenerator = nage::Make<nage::MarkovChainGenerator>(3);
        if(std::filesystem::exists("data/markov-cities.bin")) {
            m_MarkovGenerator->Load("data/markov-cities.bin");
        }
        else {
            m_MarkovGenerator->Compute("list/german-cities.txt");
            m_MarkovGenerator->Save("data/markov-cities.bin");
        }
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

    nage::PreparedGenerator<CityNameGenerator> prepared = generator->Prepare<CityNameGenerator>()
        .Edit([&](std::string name) {
            name[0] = toupper(name[0]);
            return name;
        })
        .Generate([&](CityNameGenerator* generator){
            return generator->Generate();
        });
    
    for(int i = 0; i < 10; i++)
        printf("%s\n", prepared.Get().c_str());

    nage::Free();
    return 0;
}