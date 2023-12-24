/****************************************************************************
*                                                                           *
*   NAGE : an extensive name generator                                      *
*                                                                           *
*   Made by Xorrad <monsieurs.aymeric@gmail.com>                            *
*   This code is licensed under MIT license (see LICENSE for details)       *
*                                                                           *
****************************************************************************/

#pragma once

#ifndef NAGE_CPP
#define NAGE_CPP

/***********************************************************
*                         HEADERS                          *
***********************************************************/

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include <utility>
#include <fstream>
#include <limits.h>
#include <string.h>
#include <filesystem>

/***********************************************************
*                    MACROS/DEFINES                        *
***********************************************************/

namespace nage {

    /***********************************************************
    *                  FORWARD DECLARATIONS                    *
    ***********************************************************/

    struct Handler;
    class Generator;
    template<typename G> class PreparedGenerator;
    class ListGenerator;
    class MarkovChainGenerator;
    class TemplateGenerator;

    extern Handler* g_Handler;

    /***********************************************************
    *                      GENERATORS                          *
    ***********************************************************/

    class Generator {
        public:
            Generator();
            ~Generator() = default;
            
            template<typename G> PreparedGenerator<G> Prepare();
            virtual std::string Generate() = 0;
        private:
    };

    // template<typename G, typename = typename std::enable_if<std::is_base_of<Generator, G>::value>::type>
    template<typename G>
    class PreparedGenerator {
        public:
            PreparedGenerator(G* generator);

            PreparedGenerator Filter(std::function<bool(const std::string&)> pred);
            PreparedGenerator Edit(std::function<std::string(std::string)> mod);
            PreparedGenerator Generate(std::function<std::string(G*)> generate);
            std::string Get();
        private:
            G* m_Generator;
            std::vector<std::function<bool(const std::string&)>> m_Filters;
            std::vector<std::function<std::string(std::string)>> m_Modifiers;
            std::function<std::string(G*)> m_Generate;
    };

    class ListGenerator : public Generator {
        public:
            ListGenerator();
            ListGenerator(const std::vector<std::string>& tokens);
            ListGenerator(const std::string& fileName);

            virtual std::string Generate() override;

            void Add(std::string token);
            void AddFromList(const std::vector<std::string>& tokens);
            void AddFromFile(const std::string& fileName);
        private:
            std::vector<std::string> m_Tokens;
    };

    class MarkovChainGenerator : public Generator {
        public: 
            MarkovChainGenerator(int order);
            MarkovChainGenerator(int order, const std::string& fileName);

            virtual std::string Generate() override;
            
            void Load(const std::string& fileName);
            void Save(const std::string& fileName);
            void Compute(const std::string& fileName);
            void LoadCacheOrCompute(const std::string& cacheFileName, const std::string& fileName);
        private:
            std::map<std::string, std::map<std::string, double>> m_Probabilities;
            int m_Order;
    };

    class TemplateGenerator : public Generator {
        public:
            TemplateGenerator();
            TemplateGenerator(const std::string& fileName);

            std::string Generate(const std::string& expr);
            virtual std::string Generate() override; 

            void LoadTemplates(const std::string& fileName);
        private:
            std::map<std::string, std::vector<std::string>> m_Templates;
    };

    /***********************************************************
    *                        HANDLER                           *
    ***********************************************************/

    struct Handler {
        std::map<uint32_t, std::unique_ptr<Generator>> generators;
    };

    /***********************************************************
    *                    GLOBAL VARIABLES                      *
    ***********************************************************/

    inline Handler* g_Handler;

    /***********************************************************
    *                   GLOBAL FUNCTIONS                       *
    ***********************************************************/
    
    void Init();
    void Free();
    Handler* GetHandler();

    template<typename T, typename... Args> std::unique_ptr<T> Make(Args&&... args);
    template<typename T> T& Get(uint32_t key);
    void Put(uint32_t key, std::unique_ptr<Generator> generator);

    namespace string {
        size_t CharLength(char ch);
        size_t StrLength(const std::string& str);
    }

    namespace file {
        template <typename T> T Read(std::ifstream& file);
        template <typename T> T Read(std::ifstream& file, size_t length);
        template <typename T> void Write(std::ofstream& file, const T& value);
        template <typename T> void Write(std::ofstream& file, const T& value, size_t length);
    }
   
   /***********************************************************
    *                   INLINE DEFINITIONS                     *
    ***********************************************************/

    /***********************************************************
    *                   GLOBAL FUNCTIONS                       *
    ***********************************************************/

    inline void Init() {
        if(g_Handler != NULL)
            return;
        g_Handler = new Handler();
    }

    inline void Free() {
        if(g_Handler == NULL)
            return;
        delete g_Handler;
    }

    inline Handler* GetHandler() {
        return g_Handler;
    }

    template<typename T, typename... Args>
    inline std::unique_ptr<T> Make(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    inline T& Get(uint32_t key) {
        return *(static_cast<T*>(g_Handler->generators.at(key).get()));
    }

    inline void Put(uint32_t key, std::unique_ptr<Generator> generator) {
        g_Handler->generators[key] = std::move(generator);
    }

    inline size_t string::CharLength(char ch) {
        if((0b00000001 & (ch >> 7)) == 0b0)
            return 1;
        if((0b00000111 & (ch >> 5)) == 0b110)
            return 2;
        if((0b00001111 & (ch >> 4)) == 0b1110)
            return 3;
        if((0b00011111 & (ch >> 3)) == 0b11110)
            return 4;
        return 1;
    }

    inline size_t string::StrLength(const std::string& str) {
        size_t len = 0;
        for(size_t i = 0; i < str.length(); i += CharLength(str[i]))
            len++;
        return len;
    }

    template <typename T>
    inline T file::Read(std::ifstream& file) {
        return Read<T>(file, sizeof(T));
    }
    
    template <typename T>
    inline T file::Read(std::ifstream& file, size_t length) {
        char* bytes = Read<char*>(file, sizeof(T));
        T value;
        memcpy((void*) &value, &bytes[0], sizeof(T));
        delete[] bytes;
        return value;
    }
    
    template <>
    inline char* file::Read<char*>(std::ifstream& file, size_t length) {
        char* bytes = new char[length];
        file.read(bytes, length);
        return bytes;
    }

    template <>
    inline std::string file::Read<std::string>(std::ifstream& file) {
        size_t length = Read<uint16_t>(file);
        char* bytes = Read<char*>(file, length);
        std::string value = std::string(bytes, length);
        delete[] bytes;
        return value;
    }
    
    template <typename T>
    inline void file::Write(std::ofstream& file, const T& value) {
        Write<T>(file, value, sizeof(T));
    }

    template <typename T>
    inline void file::Write(std::ofstream& file, const T& value, size_t length) {
        file.write((char*) &value, length);
    }

    template <>
    inline void file::Write<std::string>(std::ofstream& file, const std::string& value) {
        Write<uint16_t>(file, value.size());
        file.write(value.data(), value.size());
    }

    /***********************************************************
    *                  GENERATOR FUNCTIONS                     *
    ***********************************************************/
    
    inline Generator::Generator() {

    }

    template<typename G> inline PreparedGenerator<G> Generator::Prepare() {
        return PreparedGenerator<G>(static_cast<G*>(this));
    }

    template<typename G> inline PreparedGenerator<G>::PreparedGenerator(G* generator) {
        m_Generator = generator;
    }

    template<typename G> inline PreparedGenerator<G> PreparedGenerator<G>::Filter(std::function<bool(const std::string&)> pred) {
        m_Filters.push_back(pred);
        return *this;
    }

    template<typename G> inline PreparedGenerator<G> PreparedGenerator<G>::Edit(std::function<std::string(std::string)> mod) {
        m_Modifiers.push_back(mod);
        return *this;
    }
    
    template<typename G> inline PreparedGenerator<G> PreparedGenerator<G>::Generate(std::function<std::string(G*)> generate) {
        m_Generate = generate;
        return *this;
    }

    template<typename G> inline std::string PreparedGenerator<G>::Get() {
        std::string token = "";
        bool isValid = true;

        //TODO: replace infinite loop
        do {
            isValid = true;
            token = m_Generate(m_Generator);
            for(auto& pred : m_Filters)
                isValid &= pred(token);
        } while(!isValid);

        for(auto& mod : m_Modifiers)
            token = mod(token);

        return token;
    }

    /***********************************************************
    *                     LIST GENERATOR                       *
    ***********************************************************/
   
    inline ListGenerator::ListGenerator() {    
    }

    inline ListGenerator::ListGenerator(const std::vector<std::string>& tokens) {
        AddFromList(tokens);
    }

    inline ListGenerator::ListGenerator(const std::string& fileName) {
        AddFromFile(fileName);
    }

    inline std::string ListGenerator::Generate() {
        if(m_Tokens.empty())
            return "";
        return m_Tokens.at(rand()%m_Tokens.size());
    }

    inline void ListGenerator::Add(std::string token) {
        m_Tokens.push_back(token);
    }

    inline void ListGenerator::AddFromList(const std::vector<std::string>& tokens) {
        //TODO: check for duplicates?
        for(auto token : tokens)
            m_Tokens.push_back(token);
    }

    inline void ListGenerator::AddFromFile(const std::string& fileName) {
        std::ifstream file(fileName);
        if(!file)
            return;
        std::string line;
        while(getline(file, line))
            m_Tokens.push_back(line);
        file.close();
    }

    /***********************************************************
    *                 MARKOV CHAIN GENERATOR                   *
    ***********************************************************/
    
    inline MarkovChainGenerator::MarkovChainGenerator(int order) {
        m_Order = order;
    }

    inline MarkovChainGenerator::MarkovChainGenerator(int order, const std::string& fileName) {
        m_Order = order;
    }

    inline std::string MarkovChainGenerator::Generate() {
        if(m_Probabilities.empty())
            return  "";
        
        std::string token = "\002";
        const int maxLength = 10;

        for(int i = 0; i < maxLength; i++) {
            double r = (double) rand() / (double) INT_MAX;

            for(int k = m_Order; k > 1; k--) {
                int start = std::min((int) token.length()-1, std::max(0, i+1 - k));
                int len = std::min((int) token.size() - start, k);
                std::string chunk = token.substr(start, len);

                // printf("[%d] r=%f\trange=%d-%d\tchunk=%s\tentries=%d\n", i, r, start, (start + len-1), chunk.c_str(), m_Probabilities[chunk].size());

                double j = 0;
                for(auto [ch, p] : m_Probabilities[chunk]) {
                    // printf("%s -> %f\n", ch.c_str(), p);
                    if(j < r && r < j+p) {
                        token += ch;
                        break;
                    }
                    j += p;
                }
                if(m_Probabilities[chunk].size() > 0)
                    break;
            }

            if(token[token.size()-1] == '\003') {
                break;
            }
        }

        // Remove 'Start of Text' and 'End of Text' characters.
        token.erase(0, 1);
        token.pop_back();

        return token;
    }

    inline void MarkovChainGenerator::Load(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::binary);
        if(!file)
            return;

        // printf("loading probabilities from %s\n", fileName.c_str());

        m_Probabilities.clear();
        
        size_t count = file::Read<size_t>(file);

        // printf("count=%ld\n", count);

        for(size_t i = 0; i < count; i++) {
            std::string chunk = file::Read<std::string>(file);
            size_t charactersCount = file::Read<size_t>(file);
            // printf("[%ld] chunk=%s\tcharCount=%ld\n", i, chunk.c_str(), charactersCount);

            m_Probabilities[chunk] = {};

            for(size_t j = 0; j < charactersCount; j++) {
                std::string ch = file::Read<std::string>(file);
                double p = file::Read<double>(file);
                m_Probabilities[chunk][ch] = p;
            }
        }

        file.close();

        // printf("finished loading probabilities\n");
    }

    inline void MarkovChainGenerator::Save(const std::string& fileName) {
        std::ofstream file(fileName, std::ios::binary);
        if(!file)
            return;
        
        // printf("saving probabilities to %s\n", fileName.c_str());

        file::Write<size_t>(file, m_Probabilities.size());

        for(auto [chunk, characters] : m_Probabilities) {
            file::Write<std::string>(file, chunk);
            file::Write<size_t>(file, characters.size());
            for(auto [ch, p] : characters) {
                file::Write<std::string>(file, ch);
                file::Write<double>(file, p);
            }
        }
        
        file.close();

        // printf("finished saving to file\n");
    }

    inline void MarkovChainGenerator::Compute(const std::string& fileName) {
        std::ifstream file(fileName);
        if(!file)
            return;
        std::map<std::string, std::pair<int, std::map<std::string, int>>> occurrences;
        std::string line;

        // printf("started computation using file %s\n", fileName.c_str());

        while(getline(file, line)) {

            // Add 'Start of Text' and 'End of Text' characters.
            line = "\002" + line + "\003";

            // printf("processing: %s\n", line.c_str());

            for(size_t i = 0; i < line.length();) {
                size_t charLength = string::CharLength(line[i]);
                std::string ch = line.substr(i, charLength);
                std::string chunk = line.substr(i, charLength);

                for(int j = 0; j < m_Order; j++) {
                    if(i + chunk.length() >= line.length())
                        break;
                    size_t lastCharLength = string::CharLength(line[i + chunk.length()]);
                    std::string lastChar = line.substr(i + chunk.length(), lastCharLength);

                    // printf("[%d,%d] char=%s (%d)\tchunk=%s (%d)\n", i, j, lastChar.c_str(), lastCharLength, chunk.c_str(), chunk.length());

                    if(occurrences.count(chunk) == 0)
                        occurrences[chunk] = {0, {}};
                    if(occurrences[chunk].second.count(lastChar) == 0)
                        occurrences[chunk].second[lastChar] = 0;
                    occurrences[chunk].second[lastChar]++;
                    occurrences[chunk].first++;

                    chunk += lastChar;
                }
                i += charLength;
            }
        }

        for(auto [chunk, characters] : occurrences) {
            for(auto [ch, count] : characters.second) {
                double probability = (double) count / (double) characters.first;
                m_Probabilities[chunk][ch] = probability;
            }
        }

        file.close();

        // printf("finished computation\n");
    }

    inline void MarkovChainGenerator::LoadCacheOrCompute(const std::string& cacheFileName, const std::string& fileName) {
        if(std::filesystem::exists(cacheFileName)) {
            //TODO: check if source list was updated or cache file invalid.
            Load(cacheFileName);
        }
        else {
            Compute(fileName);
            Save(cacheFileName);
        }
    }

    /***********************************************************
    *                   TEMPLATE GENERATOR                     *
    ***********************************************************/

    inline TemplateGenerator::TemplateGenerator() {
    }
    
    inline TemplateGenerator::TemplateGenerator(const std::string& fileName) {
        LoadTemplates(fileName);
    }

    inline std::string TemplateGenerator::Generate(const std::string& expr) {
        std::string str = "";

        for(size_t i = 0; i < expr.size(); i += string::CharLength(expr[i])) {
            std::string e = expr.substr(i, string::CharLength(expr[i]));
            if(m_Templates.count(e) == 0 || m_Templates[e].size() == 0) {
                str += e;
                continue;
            }
            str += m_Templates[e][rand() % m_Templates[e].size()];
        }

        return str;
    }

    inline std::string TemplateGenerator::Generate() {
        return Generate("ss");
    }

    inline void TemplateGenerator::LoadTemplates(const std::string& fileName) {
        m_Templates.clear();

        std::ifstream file(fileName);
        if(!file)
            return;

        enum Scope { KEY, VALUE };
        std::string key;
        Scope scope = Scope::KEY;

        char c;
        std::string str;

        while(file.get(c)) {
            // Get UTF-8 character by looping over following bytes.
            std::string ch = std::string(1, c);
            for(size_t i = 1; i < string::CharLength(c); i++) {
                if(!file.get(c))
                    goto End;
                ch += c;
            }
            // Syntax: key=value1,value2,value3;
            // The scope can be key or value.
            // - Operations for Scope::KEY are concatenating the key (a,b,c...) or switching to values (=)
            // - Operations for Scope::VALUE are concatenating the value (a,b,c...), switching to next value (,)
            //   or switching to next key (;)
            // the following characters are not allowed in keys or values: ',' ';' '\n' ' ' '\'' '-'
            switch(scope) {
                case Scope::KEY:
                    if(ch == "=") {
                        key = str;
                        str = "";
                        m_Templates[key] = {};
                        scope = Scope::VALUE;
                    }
                    else if(ch != "," && ch != ";" && ch != "\n" && ch != " " && ch != "'" && ch != "-") {
                        str += ch;
                    }
                    break;
                case Scope::VALUE:
                    if(ch == "," || ch == ";") {
                        m_Templates[key].push_back(str);
                        str = "";
                        if(ch == ";")
                            scope = Scope::KEY; 
                    }
                    else if(ch != "\n") {
                        str += ch;
                    }
                    break;
            }
        }
        
        End:
        file.close();
    }

}
#endif
