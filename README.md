# nage
An extensive name generator for worldbuilding and roleplay.

## Token

A token can be a **word** or a **sentence** depending on the generator.

For instance, in a character name generator, the tokens to be generated are forenames and lastnames. In a family motto generator, the tokens would be sentences. For a generator based on a list (from a file), each entry/line is a token.

## Generators

A generator is used to generate a name/word.

**A generator can hold other generators:**
```
CharacterNameGenerator:  
 -> p=0.5      | AncestryNameGenerator(dynasty, gender)  
 -> p=0.3      | ListNameGenerator(culture, gender)  
 -> p=0.2      | RinkworksGenerator(gender)  
 -> !hasFamily | FamilyNameGenerator(culture)  
```

**A generator has several functions available:**
- `filter()` => bool : add conditions for a name to be picked
- `generate(...)` => string : generate a name. Arguments can be different depending on the generator (i.e name vs word)


## Manager/Handler

The goal of nage generators handler is to store initialized generators into a map to make them accessible anytime and from anywhere without having to reinitialize it (and thus avoid reading/computing the tokens).

Generators are stored in a hashmap using int as key (enums on the application side).