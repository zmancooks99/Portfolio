//
//  Prop.hpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#ifndef Prop_hpp
#define Prop_hpp
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Solver_n.hpp"

//*************************************************************************************************
//                                      OBJECTS
//*************************************************************************************************

//CLASS Prop
//
//INVARIANTS:
//  NONE
class Prop
{
    //*************************************************************************************************
    //                                      PUBLIC FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
    
public:
    
    //***************************************************
    //              CTORS, ASSIGNMENT OPS, DCTOR
    //***************************************************
    
    //  DEFAULT CTOR
    //
    Prop();
    
    //  CTOR WITH PARAMS
    //
    Prop(std::vector<int> play, std::vector<std::string> sus,std::vector<std::string> wpn, std::vector<std::string> rms, std::vector<std::string> cards);
    
    //  Copy CTOR
    //
    
    Prop(const Prop& p);
    //  DEFAULT DCTOR
    //  DOES NOT THROW
    ~Prop() = default;
    
    //ASSIGNMENT OPERATOR
    //
    Prop& operator=(const Prop& p);
    
    //***************************************************
    //              MEMBER FUNCTIONS
    //***************************************************
    std::string addClause(std::vector<int>& c);
    std::string addClause(std::vector<std::string>& c);
    std::string addTestClause(std::vector<int>& c);
    std::string addTestClause(std::vector<std::string>& c);
    int toUnique(int player, int card); //Generates a unique ID that represents each player-card pair
    int has(int player, int card);
    int nHas(int player, int card);
    std::string getCNF() const;
    std::string setCNF(std::string cnf);
    int getNumClauses() const;
    int getNumVariables() const;
    int getNumPlayers() const;
    int getNumCards() const;
    std::vector<std::string> getSuspects() const;
    std::vector<std::string> getWeapons() const;
    std::vector<std::string> getRooms() const;
    std::vector<std::string> getCards() const;
    std::vector<int> getPlayers() const;
    std::string addHead();
    int checkLiteral(int id); //Returns 0 for unsat and 1 for sat
    std::string checkSAT(); //Check if the current CNF is satisfiable
    static const bool TESTING = FALSE;
    
    //*************************************************************************************************
    //                                      PRIVATE FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
    
private:
    std::string _CNF;
    int _numClauses;
    int _numVariables;
    int _numPlayers;
    int _numPos;
    int _numCards;
    std::vector<int> _players;
    std::vector<std::string> _suspects;
    std::vector<std::string> _weapons;
    std::vector<std::string> _rooms;
    std::vector<std::string> _cards;
};

#endif /* Prop_hpp */
