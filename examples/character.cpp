#include "../nage.hpp"

enum Generators {
    CHARACTER_NAME,
};

enum Gender {
    MALE,
    FEMALE
};

class CharacterNameGenerator : public nage::Generator {
public:
    std::string Generate(Gender gender/*, Culture culture*/) {
        static const std::map<Gender, std::vector<std::string>> names = {
            {Gender::MALE, {"Karl", "Tim", "Ragnar"}},
            {Gender::FEMALE, {"Ana", "Marie", "Jeanne"}}
        };
        return names.at(gender).at(rand()%names.at(gender).size());
    }
    virtual std::string Generate() override {
        return Generate((Gender)(rand()%2));
    }
private:
};

int main() {
    srand(time(NULL));

    // Initialize nage (create context, prepare handler)
    nage::Init();

    // Add a generator to handler with id CHARACTER_NAME
    nage::Put(Generators::CHARACTER_NAME, nage::Make<CharacterNameGenerator>());

    nage::PreparedGenerator<CharacterNameGenerator> prepared = nage::Get<CharacterNameGenerator>(Generators::CHARACTER_NAME)
        .Prepare<CharacterNameGenerator>()
        .Filter([&](const std::string& name){
            return name.size() < 10;
            return true;
        })
        .Edit([&](std::string name) {
            return name + "ius";
        })
        .Generate([&](CharacterNameGenerator* generator){
            return generator->Generate(Gender::MALE/*, culture*/);
        });

    // Generate names
    std::string name = prepared.Get();
    printf("%s\n", name.c_str());

    nage::Free();
    return 0;
}