# nage

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/xorrad/nage)](https://github.com/xorrad/nage/issues)
[![GitHub stars](https://img.shields.io/github/stars/xorrad/nage.svg?style=flat&label=stars)](https://github.com/xorrad/nage)

A powerful and customizable generators manager for creating diverse and unique names based on user-defined rules and patterns. The goal is to make it a useful and reliable tool for game developpment, worldbuilding or roleplaying.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Introduction

*nage* empowers you to generate names for a wide range of purposes with its flexible and dynamic approach.

My motivations behing making this project were to be able to generate a wide variety of things, including character names, location names, city names or even words to make a procedural constructed language. I am also planning on enabling generation of any type of data such as for instance: numbers, objects (i.e Character)...

## Features

- **Customizable Patterns:** Define your own naming patterns and rules.
- **Diversity:** Generate names for different cultures, genres, or themes.
- **Randomness Control:** Fine-tune the level of randomness in generated names.
- **Extensibility:** Easily extend the generator with custom modules and datasets.
- **Header-Only:**: Add only one file to your project to start using *nage*.

## Installation

Download the header file: [nage.hpp](https://raw.githubusercontent.com/Xorrad/nage/master/nage.hpp), then include it into your project and initialize and free *nage* as in the following example:

```cpp
#include "nage.hpp"

int main() {
    nage::Init();
    // your code...
    nage::Free();
    return 0;
}
```

## Usage

### Predefined Generators   
Initialize predefined generators as follows:
```cpp
std::unique_ptr<nage::ListGenerator> myListGenerator = nage::Make<nage::ListGenerator>("list/french-names.txt");
std::string name1 = myListGenerator->Generate();

std::unique_ptr<nage::MarkovChainGenerator> myMarkovGenerator = nage::Make<nage::MarkovChainGenerator>(3);
myMarkovGenerator->LoadCacheOrCompute("data/markov-cities.bin", "list/german-cities.txt");
std::string name2 = myMarkovGenerator->Generate();
```

### Use Generators Anywhere in Your Code
Give generators to the `Handler` to make them accessible anywhere in your code:
```cpp
int generatorId = 0;
nage::Put(generatorId, nage::Make<nage::ListGenerator>("list/french-names.txt"));
std::string name = nage::Get<nage::ListGenerator>(generatorId)->Generate();
```

### PreparedGenerator for Advanced Name Generation
Create a `nage::PreparedGenerator` to apply filters and modifiers to generated names:to add filters and modifiers on generated names:
```cpp
nage::PreparedGenerator<nage::ListGenerator> prepared = myListGenerator->Prepare<nage::ListGenerator>()
    .Filter([&](const std::string& name){
        return name.size() == 10;
    })
    .Edit([&](std::string name) {
        name[0] = toupper(name[0]);
        return name;
    })
    .Generate([&](nage::ListGenerator* generator){
        return generator->Generate();
    });

std::string name = prepared.Get();
```

### Custom Generator
Create a custom generator by extending the `nage::Generator` class:
```cpp
class MyGenerator : public nage::Generator {
public:
    virtual std::string Generate() override {
        std::string str;
        // your code...
        return str;
    }
};
```

## Examples

There are a few examples available in the [examples](https://github.com/Xorrad/nage/tree/master/examples) directory. Do not hesitate to check them to learn more about using predefined generators and making new ones.

## Contributing

Contributions to the project are highly appreciated! There are several ways to get involved: you can contribute by reporting any issues you encounter, suggesting new features that could enhance the project, or even by actively participating in the development process through the submission of pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](https://raw.githubusercontent.com/Xorrad/nage/master/LICENSE) file for details.