#include "generator.hpp"

float randomFloat()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);
    return dist(e2);
}

Generator::Generator()
{
    this->depth = 1;
    this->init();
}

Generator::Generator(int depth)
{
    this->depth = depth;

    this->init();
}

Generator::Generator(int depth, std::map<std::string, Chunk> probabilities, float averageLength, int minLength, int maxLength)
{
    this->depth = depth;
    this->probabilities = probabilities;
    this->averageLength = averageLength;
    this->minLength = minLength;
    this->maxLength = maxLength;
}

Generator::~Generator()
{
}

void Generator::init()
{
    this->probabilities.clear();

    this->count = 0;
    this->averageLength = 0;
    this->minLength = 10000;
    this->maxLength = 0;
}

float Generator::getProbability(std::string input, std::string output)
{
    if(this->probabilities.count(input) == 0.f)
        return 0.f;

    if(this->probabilities[input].outputs.count(output) == 0.f)
        return 0.f;

    return this->probabilities[input].outputs[output];
}

std::string Generator::getChunk(std::string input, int length, int index)
{
    std::string chunk = "";

    for(int i = index-length; i<index; i++)
    {
        if(i < 0)
            chunk += " ";
        else
            chunk += input[i];
    }

    return chunk;
}

bool Generator::isValid(char c)
{
    std::string characters = CHAR_ALPHABET;

    for(char l : characters)
        if(l == c)
            return true;

    return false;
}

float Generator::getEndingProbability(std::string input, int length)
{
    if(length < this->minLength)
        return 0.f;

    float endingLength = this->probabilities[input].averageEndingLength;
    float probability = this->probabilities[input].endingProbability;

    if(length < (0.75f*endingLength))
        return 0.f;

    float lengthMultiplier = pow((float) length / (endingLength), 2);
    float finalProbability = probability*lengthMultiplier;

    return finalProbability;
}

char Generator::randomChar(std::string input, int length)
{
    float count = 0.f;
    float ran = randomFloat();
    float endingProbability = this->getEndingProbability(input, length);

    if(DEBUG)
    {
        msleep(500);
        std::cout << "pre: \"" << input << "\"\tlen:" << input.length() << "\tran: " << ran << "\tending: " << endingProbability << std::endl;
    }

    if(randomFloat() <= endingProbability)
    {
        return CHAR_END;
    }

    if(this->probabilities.count(input) == 0)
        return CHAR_UNKNOWN;

    for(auto nextChar : this->probabilities[input].outputs)
    {
        if(DEBUG)
            std::cout << "\tchar: " << nextChar.first[0] << "\tinterval: " << count << "\t" << (count+nextChar.second) << "\t" << (ran >= count && ran <= (count+nextChar.second)) << std::endl;

        if(ran >= count && ran <= (count+nextChar.second))
            return nextChar.first[0];

        count += nextChar.second;
    }

    return '!';
}

std::string Generator::randomString()
{
    std::string s = "";
    int length = 1;
    int i = 0;

    while(i < length)
    {   
        for(int subLength = this->depth; subLength > 0; subLength--)
        {
            std::string input = (i > 0) ? this->getChunk(s, subLength, i) : std::string(1, CHAR_START);
            char nextChar = this->randomChar(input, i);

            if(nextChar == CHAR_END)
            {
                s[0] = _toupper(s[0]);
                return s;
            }

            if(nextChar != CHAR_UNKNOWN)
            {
                length++;
                i++;
                s += nextChar;
                break;
            }
        }
    }

    s[0] = _toupper(s[0]);

    return s;
}

void Generator::loadFromList(std::string path)
{
    this->init();

    std::ifstream file(path);

    if(file.is_open())
    {
        std::string line;

        while(getline(file, line))
        {
            this->averageLength += line.length();
            this->count++;
            this->minLength = std::min(this->minLength, int(line.length()));
            this->maxLength = std::max(this->maxLength, int(line.length()));

            for(int i=0; i<=line.length(); i++)
            {
                if(line.length() == 0) break;
                if(i < line.length()) line[i] = tolower(line[i]);

                for(int subLength=1; subLength <= this->depth; subLength++)
                {
                    std::string input = (i > 0) ? this->getChunk(line, subLength, i) : std::string(1, CHAR_START);

                    if(this->probabilities.count(input) == 0)
                    {
                        this->probabilities[input].endingProbability = 0.f;
                        this->probabilities[input].averageEndingLength = 0.f;

                        this->probabilities[input].outputsCount = 0.f;
                        this->probabilities[input].endingLengthCount = 0.f;
                    }

                    if(i < line.length() && isValid(line[i]))
                    {
                        std::string output = std::string(1, line[i]);

                        if(this->probabilities[input].outputs.count(output) == 0)
                            this->probabilities[input].outputs[output] = 0;

                        this->probabilities[input].outputs[output]++;
                        this->probabilities[input].outputsCount++;
                    }
                    else
                    {
                        this->probabilities[input].averageEndingLength += line.length();
                        this->probabilities[input].endingLengthCount++;
                        this->probabilities[input].endingProbability++;
                    }
                }
            }
        }

        std::ofstream output("probabilities.txt");

        this->averageLength /= this->count;
        output << "mean: " << this->averageLength << std::endl;
        output << "min: " << this->minLength << std::endl;
        output << "max: " << this->maxLength << std::endl;

        for(auto input : this->probabilities)
        {
            int count = this->probabilities[input.first].outputsCount;
            this->probabilities[input.first].endingProbability /= std::max(1, int(count + this->probabilities[input.first].endingProbability));
            this->probabilities[input.first].averageEndingLength /= std::max(1, int(this->probabilities[input.first].endingLengthCount));

            output << "char: \"" << input.first << "\"\ttotal: " << count << "\t ending: " << this->probabilities[input.first].endingProbability << "\t endingLength: " << this->probabilities[input.first].averageEndingLength << std::endl;

            for(auto nextChar : input.second.outputs)
            {
                this->probabilities[input.first].outputs[nextChar.first] /= count;

                //std::cout << l.first << "\t" << r.first << "\t" << this->probabilities[l.first][r.first] << std::endl;
                output << "\t\t\"" << nextChar.first << "\"\t" << this->probabilities[input.first].outputs[nextChar.first] << std::endl;
            }
        }

        output.close();
    }
}

void Generator::loadFromFile(std::string path)
{
    std::ifstream file(path);

    if(file.is_open())
    {
        std::string line = "";
        int count = 0;

        while(getline(file, line))
        {
            switch(count)
            {
                case 0:
                    this->depth = stoi(line);
                    break;
                case 1:
                    this->count = stoi(line);
                    break;
                case 2:
                    this->averageLength = stof(line);
                    break;
                case 3:
                    this->minLength = stoi(line);
                    break;
                case 4:
                    this->maxLength = stoi(line);
                    break;

                default:
                    std::string input = line;

                    getline(file, line);
                    this->probabilities[input].endingProbability = stof(line);
                    
                    getline(file, line);
                    this->probabilities[input].averageEndingLength = stof(line);

                    getline(file, line);
                    this->probabilities[input].outputsCount = stof(line);

                    getline(file, line);
                    this->probabilities[input].endingLengthCount = stof(line);

                    getline(file, line);
                    int size = stoi(line);

                    for(int j = 0; j < size; j++)
                    {
                        getline(file, line);
                        std::istringstream stream(line);
                        std::string output;
                        std::string probability;

                        getline(stream, output, ':');
                        getline(stream, probability, ':');

                        this->probabilities[input].outputs[output] = stof(probability);
                    }
            }


            count++;
        }
    }
}

void Generator::saveToFile(std::string name)
{
    std::ofstream file("data/" + name);

    if(file.is_open())
    {
        file << this->depth << std::endl;
        file << this->count << std::endl;
        file << this->averageLength << std::endl;
        file << this->minLength << std::endl;
        file << this->maxLength << std::endl;

        for(auto input : this->probabilities)
        {
            Chunk chunk = input.second;

            file << input.first << std::endl;
            file << chunk.endingProbability << std::endl;
            file << chunk.averageEndingLength << std::endl;
            file << chunk.outputsCount << std::endl;
            file << chunk.endingLengthCount << std::endl;
            file << chunk.outputs.size() << std::endl;

            for(auto output : chunk.outputs)
            {
                file << output.first << ":" << output.second << std::endl;
            }
        }

        file.close();
    }
    else
    {
        throw std::runtime_error("Could not open file " + name);
    }
}