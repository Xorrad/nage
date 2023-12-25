#include "../nage.hpp"

int main() {
    srand(time(NULL));

    // Initialize nage (create context, prepare handler)
    nage::Init();

    std::unique_ptr<nage::TemplateGenerator> generator = nage::Make<nage::TemplateGenerator>("data/templates/rinkworks.txt");
    
    std::map<std::string, std::string> templates = {
        {"japanese", "(aka|aki|bashi|gawa|kawa|furu|fuku|fuji|hana|hara|haru|hashi|hira|hon|hoshi|ichi|iwa|kami|kawa|ki|kita|kuchi|kuro|marui|matsu|miya|mori|moto|mura|nabe|naka|nishi|no|da|ta|o|oo|oka|saka|saki|sawa|shita|shima|i|suzu|taka|take|to|toku|toyo|ue|wa|wara|wata|yama|yoshi|kei|ko|zawa|zen|sen|ao|gin|kin|ken|shiro|zaki|yuki|asa)(||||||||||bashi|gawa|kawa|furu|fuku|fuji|hana|hara|haru|hashi|hira|hon|hoshi|chi|wa|ka|kami|kawa|ki|kita|kuchi|kuro|marui|matsu|miya|mori|moto|mura|nabe|naka|nishi|no|da|ta|o|oo|oka|saka|saki|sawa|shita|shima|suzu|taka|take|to|toku|toyo|ue|wa|wara|wata|yama|yoshi|kei|ko|zawa|zen|sen|ao|gin|kin|ken|shiro|zaki|yuki|sa)"},
        {"chinese", "(zh|x|q|sh|h)(ao|ian|uo|ou|ia)(|(l|w|c|p|b|m)(ao|ian|uo|ou|ia)(|n)|-(l|w|c|p|b|m)(ao|ian|uo|ou|ia)(|(d|j|q|l)(a|ai|iu|ao|i)))"},
        {"old latin place", "sv(nia|lia|cia|sia)"},
        {"test", "(a|b|c|d|<s>)"}
    };

    // Generate a few names
    for(auto& [name, expr] : templates) {
        printf("%s:\n", name.c_str());
        for(int i = 0; i < 5; i++)
            printf("  - %s\n", generator->Generate(expr).c_str());
        printf("\n");
    }

    nage::Free();
    return 0;
}