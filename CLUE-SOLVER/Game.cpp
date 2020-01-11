//
//  Game.cpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#include "Game.hpp"
using std::string;
using std::vector;
using std::distance;
using std::cout;
using std::endl;

const int DECK = 21;
const vector<string> ROOMS = { "hall", "lounge", "diningroom",
    "kitchen", "ballroom", "conservatory",
    "billardroom","library","study" };
const vector<string> SUSPECTS = { "colmustard", "profplum", "mrgreen",
    "mrspeacock","missscarlet","mrswhite" };
const vector<string> WEAPONS = { "knife", "candlestick", "wrench",
    "revolver", "rope", "leadpipe" };

Game::Game() : _rooms(ROOMS), _suspects(SUSPECTS), _weapons(WEAPONS), _numCards(DECK)
{
    for (string room : _rooms)
    {
        _cards.push_back(room);
    }
    for (string suspect : _suspects)
    {
        _cards.push_back(suspect);
    }
    for (string weapon : _weapons)
    {
        _cards.push_back(weapon);
    }
    setTurn(0);
    if(TESTING)
    {
        cout << "CREATING GAME OBJECT(c1): " << this << endl;
        cout << ">>Game(c1) The cards are:" << endl;
        for(int i = 0; i < _numCards; i++)
        {
            cout << i << " : " << _cards.at(i) << endl;
        }
        cout << ">>Game(c1) _getPlayers returns: " << endl;
        for(size_t i = 0; i < getPlayers().size(); i++)
        {
            cout << "\t" << getPlayers().at(i) << endl;
        }
    }
}

Game::Game(int numPlayers, int playerNum) : _numPlayers(numPlayers), _playerNum(playerNum), _numPos(numPlayers+1), _rooms(ROOMS), _suspects(SUSPECTS), _weapons(WEAPONS), _numCards(DECK)
{
    for(int i = 0; i < numPlayers; i++)
    {
        _players.push_back(i);
    }
    if(TESTING)
    {
        cout << "CREATING GAME OBJECT(c2): "<< this << endl;
        cout << ">>Game _numPlayers initialized to " << _numPlayers << endl;
        cout << ">>Game _players.size() is " << _players.size() << endl;
    }
    for (string room : _rooms)
    {
        _cards.push_back(room);
    }
    for (string suspect : _suspects)
    {
        _cards.push_back(suspect);
    }
    for (string weapon : _weapons)
    {
        _cards.push_back(weapon);
    }
    if(TESTING)
    {
        cout << ">>Game(c2) The cards are:" << endl;
        for(int i = 0; i < _numCards; i++)
        {
            cout << i << " : " << _cards.at(i) << endl;
        }
        cout << ">>Game(c2) _getPlayers returns: " << endl;
        for(size_t i = 0; i < getPlayers().size(); i++)
        {
            cout << "\t" << getPlayers().at(i) << endl;
        }
    }
    setTurn(0);
}

Game::Game(const Game& g) : _numCards(g._numCards)
{
    if(TESTING)
    {
        cout << "CREATING GAME OBJECT(cc): " << this << endl;
    }
    throw std::runtime_error("Cannot copy game");
}

int Game::getCardIndex(string c)
{
    return (int)(distance(_cards.begin(), find(_cards.begin(), _cards.end(), c))); //Precision is lost here, but it doesn't matter since the size of _cards will always be 21
}

void Game::defaultAssumptions()
{
    //Each card must be used at least once
    if(TESTING)
        cout << ">>Game defaultAssignments: Each card must be used once:" << endl;
    vector<int> clause;
    for(int c = 0; c < _numCards; c++)
    {
        for(int p = 0; p < _numPos; p++)
        {
            int s = _prop.has(p, c);
            clause.push_back(s);
        }
        _prop.addClause(clause);
        clause.clear();
    }
    
    

    /*Each card can only be in one position
     *This set of loops ends up generating something like this. If we had cards A,B,C,D,E and players 1,2,3,4:
     *      NOT(A1) OR NOT(A2)
     *  AND NOT(A1) OR NOT(A3)
     *  AND NOT(A1) OR NOT(A4)
     *  AND NOT(A2) OR NOT(A3)
     *  AND NOT(A2) OR NOT(A4)
     *  AND NOT(A3) OR NOT(A4)
     *  AND NOT(B1) OR NOT(B2)
     *           ...
     */
    
    if(TESTING)
        cout << ">>Game defaultAssignments: Each card can be used only once:" << endl;
    for(int c = 0; c < _numCards; c++)
    {
        for(int p = 0; p < _numPos; p++) //numPlayers +1 to account for the hidden cards
        {
            for(int pp = p+1; pp < _numPos; pp++)
            {
                clause.push_back(_prop.nHas(p, c));
                clause.push_back(_prop.nHas(pp,c));
                _prop.addClause(clause);
                clause.clear();
            }
        }
    }
    
    
    
    //At least 1 card of each type must be a mystery card
    if(TESTING)
        cout << ">>Game defaultAssignments: At least one card of each type must be mystery card:" << endl;
    for(string w : _weapons)
    {
        clause.push_back(_prop.has(_numPlayers, getCardIndex(w)));
    }
    _prop.addClause(clause);
    clause.clear();
    for(string r : _rooms)
    {
        clause.push_back(_prop.has(_numPlayers, getCardIndex(r)));
    }
    _prop.addClause(clause);
    clause.clear();
    for(string s : _suspects)
    {
        clause.push_back(_prop.has(_numPlayers, getCardIndex(s)));
    }
    _prop.addClause(clause);
    clause.clear();
    
    
    
    //No 2 cards in each category can be the mystery cards
    if(TESTING)
        cout << ">>Game defaultAssignments: No 2 cards of the same type can be mystery cards:" << endl;
    for(size_t c = 0; c < _weapons.size(); c++)
    {
        int wi = getCardIndex(_weapons.at(c));
        for(size_t cc = c+1; cc < _weapons.size(); cc++)
        {
            int wii = getCardIndex(_weapons.at(cc));
            int mysteryIndex = _numPlayers;
            clause.push_back(_prop.nHas(mysteryIndex, wi));
            clause.push_back(_prop.nHas(mysteryIndex, wii));
            _prop.addClause(clause);
            clause.clear();
        }
    }
    for(size_t c = 0; c < _rooms.size(); c++)
    {
        int ri = getCardIndex(_rooms.at(c));
        for(size_t cc = c+1; cc < _rooms.size(); cc++)
        {
            int rii = getCardIndex(_rooms.at(cc));
            int mysteryIndex = _numPlayers;
            clause.push_back(_prop.nHas(mysteryIndex, ri));
            clause.push_back(_prop.nHas(mysteryIndex, rii));
            _prop.addClause(clause);
            clause.clear();
        }
    }
    for(size_t c = 0; c < _suspects.size(); c++)
    {
        int si = getCardIndex(_suspects.at(c));
        for(size_t cc = c+1; cc < _suspects.size(); cc++)
        {
            int sii = getCardIndex(_suspects.at(cc));
            int mysteryIndex = _numPlayers;
            clause.push_back(_prop.nHas(mysteryIndex, si));
            clause.push_back(_prop.nHas(mysteryIndex, sii));
            _prop.addClause(clause);
            clause.clear();
        }
    }
    
    //We can remove any cards we hold
    if(TESTING)
    {
        cout << ">>Game defaultAssignments: The mystery cards can't be cards we have:" << endl;
        cout << ">>Game:" << this << "(we have cards: ";
        for(string c : _myCards)
            cout << c << ", ";
        cout << ")" << endl;
    }
    for(string c : _myCards)
    {
        if(TESTING)
            cout << ">>Game:\t" << getCardIndex(c) << endl;
        clause.push_back(_prop.has(_playerNum, getCardIndex(c)));
        _prop.addClause(clause);
        clause.clear();
    }
}

int Game::pass()
{
    _myTurn = _playerNum == ++_turn;
    return 1;
}

void Game::suggest(int player, std::string suspect, std::string weapon, std::string room)
{
}

void Game::falseAccusation(int player, std::string suspect, std::string weapon, std::string room)
{
    vector<int> clause;
    //The accusing player has none of the cards they're suggesting
    clause.push_back(_prop.nHas(player, getCardIndex(suspect)));
    _prop.addClause(clause);
    clause.clear();
    clause.push_back(_prop.nHas(player, getCardIndex(weapon)));
    _prop.addClause(clause);
    clause.clear();
    clause.push_back(_prop.nHas(player, getCardIndex(room)));
    _prop.addClause(clause);
    clause.clear();
    //The mystery cards CANNOT be all of the cards the player suggested
    clause.push_back(_prop.nHas(_numPlayers, getCardIndex(suspect)));
    clause.push_back(_prop.nHas(_numPlayers, getCardIndex(weapon)));
    clause.push_back(_prop.nHas(_numPlayers, getCardIndex(room)));
}

void Game::noResponse(int player, string suspect, string weapon, string room)
{
    vector<int> clause;
    //The player who does not respond has none of the cards that were suggested
    clause.push_back(_prop.nHas(player, getCardIndex(suspect)));
    _prop.addClause(clause);
    clause.clear();
    clause.push_back(_prop.nHas(player, getCardIndex(weapon)));
    _prop.addClause(clause);
    clause.clear();
    clause.push_back(_prop.nHas(player, getCardIndex(room)));
    _prop.addClause(clause);
    clause.clear();
    
}

void Game::responds(int player, string suspect, string weapon, string room)
{
    vector<int> clause;
    //The player who responds has at least one of the cards that was suggested if not myTurn()
    clause.push_back(_prop.has(player, getCardIndex(suspect)));
    clause.push_back(_prop.has(player, getCardIndex(weapon)));
    clause.push_back(_prop.has(player, getCardIndex(room)));
    _prop.addClause(clause);
}

void Game::responds(int player, string response)
{
    vector<int> clause;
    //This is only called if myTurn() so the player definitely has the card
    clause.push_back(_prop.has(player, getCardIndex(response)));
    _prop.addClause(clause);
}

int Game::setTurn(int player)
{
    _turn = player;
    _myTurn = _turn == _playerNum;
    return _turn;
}

int Game::getTurn()
{
    return _turn;
}

vector<string> Game::getCards()
{
    return _cards;
}

vector<string> Game::getRooms()
{
    return _rooms;
}

vector<string> Game::getWeapons()
{
    return _weapons;
}

vector<string> Game::getSuspects()
{
    return _suspects;
}

vector<int> Game::getPlayers()
{
    return _players;
}

Prop& Game::getProp()
{
    return _prop;
}

void Game::setProp(const Prop& p)
{
    _prop = p;
}

void Game::setMyCards(const vector<string> cards)
{
    if(TESTING)
    {
        cout << ">>Game Setting my cards in " << this << " to:" << endl;
        for(string c : cards)
            cout << "\t" << c << endl;
    }
    _myCards = cards;
}

Game* Game::getAddress()
{
    return this;
}

bool Game::myTurn()
{
    return _myTurn;
}

int Game::getNumPlayers()
{
    return _numPlayers;
}

int Game::getNumCards()
{
    return _numCards;
}

int Game::checkLiteral(int i)
{
    return _prop.checkLiteral(i);
}

int Game::getNumVariables()
{
    return _prop.getNumVariables();
}
