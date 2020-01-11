//
//  Interpreter.hpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//
//  THIS IS THE TEXT INTERPRETER


#ifndef Interpreter_hpp
#define Interpreter_hpp
#include <string>
#include <vector>
#include "Game.hpp"

//CLASS Interpreter
//
//INVARIANTS:
//  NONE
class Interpreter
{
    //*************************************************************************************************
    //                                      PUBLIC FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
    
public:
    
    //***************************************************
    //              CTORS, ASSIGNMENT OPS, DCTOR
    //***************************************************
    
    //  DEFAULT CTOR
    //  DELETED
    Interpreter() = delete;
    
    //CTOR WITH PARAMS
    //
    Interpreter(std::shared_ptr<Game> gP);
    
    
    int interpret(std::string str);
    
    static const bool TESTING = FALSE;
    
    //*************************************************************************************************
    //                                      PRIVATE FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
    
private:
    std::shared_ptr<Game> _gameP;
    vector<string> currentSuggestion;
    bool cardsContains(Game& g, std::vector<std::string> cards);
    bool cardsContains(Game& g, std::string card);
};

#endif /* Interpreter_hpp */
