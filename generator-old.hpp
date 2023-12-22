#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <math.h>
#include <random>
#include <sstream>
#include <stdexcept>

#define msleep(ms) usleep(ms*1000)

#define DEBUG 0

#define CHAR_ALPHABET "abcdefghijklmnopqrstuvwxyz- éàèæâêôîûŷëäïöüç"
#define CHAR_START '>'
#define CHAR_END '<'
#define CHAR_UNKNOWN '!'

struct Chunk
{
    std::map<std::string, float> outputs;
    float endingProbability = 0.f;
    float averageEndingLength = 0.f;

    float outputsCount = 0.f;
    float endingLengthCount = 0.f;
};

class Generator
{
    private:
        int depth;
        std::map<std::string, Chunk> probabilities;

        int count;
        float averageLength;
        int minLength;
        int maxLength;

        std::random_device randomDevice;
        std::mt19937 randomEngine;
        std::uniform_real_distribution<> randomDistribution;

    public:
        Generator();
        Generator(int depth);
        Generator(int depth, std::map<std::string, Chunk> probabilities, float averageLength, int minLength, int maxLength);
        ~Generator();

        void init();

        float getProbability(std::string input, std::string output);
        std::string getChunk(std::string input, int length, int index);
        bool isValid(char c);
        float getEndingProbability(std::string input, int length);

        char randomChar(std::string input, int length);
        std::string randomString();

        void loadFromList(std::string path);
        void loadFromFile(std::string path);
        void saveToFile(std::string name);

        float rand();
};
#endif