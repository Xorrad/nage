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
    CharacterNameGenerator() {
        m_MaleList = nage::Make<nage::ListGenerator>("list/french-names-male.txt");
        m_FemaleList = nage::Make<nage::ListGenerator>("list/french-names-female.txt");
    }

    std::string Generate(Gender gender/*, Culture culture*/) {
        switch(gender) {
            case MALE:
                return m_MaleList->Generate();
            case FEMALE:
                return m_FemaleList->Generate();
        }
    }
    virtual std::string Generate() override {
        return Generate((Gender)(rand()%2));
    }
private:
    std::unique_ptr<nage::ListGenerator> m_MaleList;
    std::unique_ptr<nage::ListGenerator> m_FemaleList;
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
            name[0] = toupper(name[0]);
            return name;
        })
        .Generate([&](CharacterNameGenerator* generator){
            return generator->Generate(Gender::MALE/*, culture*/);
        });

    // Generate names
    for(int i = 0; i < 10; i++)
        printf("%s\n", prepared.Get().c_str());

    nage::Free();
    return 0;
}