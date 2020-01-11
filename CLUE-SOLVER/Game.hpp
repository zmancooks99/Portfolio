//
//  Game.hpp
//  ClueSAT
//
//  Created by Zachary Mason
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp
#include <string>
#include <vector>
#include <iostream>
#include "Prop.hpp"
#include "Solver_n.hpp"

//*************************************************************************************************
//                                      OBJECTS
//*************************************************************************************************

//CLASS Game
//
//INVARIANTS:
//  NONE
class Game
{
    //*************************************************************************************************
    //                                      PUBLIC FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
public:
    //***************************************************
    //              CTORS, ASSIGNMENT OPS, DCTOR
    //***************************************************
    
    //DEFAULT CTOR
    //
    Game();
    
    //DCTOR
    //DEFAULT. DOES NOT THROW;
    ~Game() = default;
    
    //CTOR WITH PARAMS
    //
    Game(int numPlayers, int playerNum);
    
    //COPY CTOR
    //ONLY INCLUDED FOR TESTING =delete for publishing
    Game(const Game& g); //DELETE BEFORE FINALIZING. ONLY FOR TESTING
    
    //***************************************************
    //              MEMBER FUNCTIONS
    //***************************************************
    
    int getCardIndex(std::string c);
    std::vector<std::string> getSuspects();
    std::vector<std::string> getWeapons();
    std::vector<std::string> getRooms();
    std::vector<std::string> getCards();
    std::vector<int> getPlayers();
    int getNumPlayers();
    int getNumCards();
    int getTurn();
    int getNumVariables();
    int setTurn(int player);
    void setMyCards(const std::vector<std::string> cards);
    Game* getAddress();             //STRICTLY USED IN TESTING CLAUSES
    
    int pass();
    int checkLiteral(int i);
    void suggest(int player, std::string suspect, std::string weapon, std::string room);
    void falseAccusation(int player, std::string suspect, std::string weapon, std::string room);
    void noResponse(int player, std::string suspect, std::string weapon, std::string room);
    void responds(int player, std::string suspect, std::string weapon, std::string room);
    void responds(int player, std::string response);
    void defaultAssumptions();
    bool myTurn();
    
    
    Prop& getProp();
    void setProp(const Prop& p);
    
    static const bool TESTING = FALSE;
    
    //*************************************************************************************************
    //                                      PRIVATE FUNCTIONS AND MEMBER VARS
    //*************************************************************************************************
private:
    bool _myTurn; //True if it is the user's turn
    int _numPlayers; //The number of players in the game
    int _numPos; //The number of position. i.e. The number of players plus one (for the mystery cards)
    int _turn;
    const int _numCards;
    std::vector<int> _players;
    const std::vector<std::string> _suspects;
    const std::vector<std::string> _weapons;
    const std::vector<std::string> _rooms;
    std::vector<std::string> _cards;
    Prop _prop;
    std::vector<std::string> _myCards;
    int _playerNum; //Your player number, assuming play starts with player 0
};
#endif /* Game_hpp */
