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
#include <map>
#include <memory>
#include <functional>
#include <utility>

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
   
}
#endif