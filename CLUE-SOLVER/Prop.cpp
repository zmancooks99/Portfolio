//
//  Prop.cpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#include "Prop.hpp"
using std::string;
using std::vector;
using std::ostringstream;
using std::to_string;
using std::ofstream;
/*FOR TESTING*/
using std::cout;
using std::endl;
/*************/



Prop::Prop() //This constructor only exists because otherwise the compiler requires it to be initialized in an initialization list in Game constructor
{
}
Prop::Prop(vector<int> play, vector<string> sus,vector<string> wpn, vector<string> rms, vector<string> cards) : _players(play), _suspects(sus), _weapons(wpn), _rooms(rms), _cards(cards), _numClauses(0)
{
    _numPlayers = (int)(_players.size());
    _numPos = _numPlayers+1;
    _numCards = (int)(_cards.size());
    _numVariables = _numPlayers * _numCards + _numCards; //numPlayers is used because the number of players is the same as the index representing the hidden cards.
    if(TESTING)
    {
        cout << ">>Prop(c) creating Prop object: " << this << endl;
        cout << ">>Prop(c) _numPlayers initialized to " << _numPlayers << endl << ">>Prop(c) _numCards initialized to " << _numCards << endl;
        cout << ">>Prop(c) _numVariables initialized to " << _numVariables << endl;
    }
}
Prop::Prop(const Prop& p) : _players(p._players), _suspects(p._suspects), _weapons(p._weapons), _rooms(p._rooms), _cards(p._cards), _numClauses(p._numClauses), _numPlayers(p._numPlayers), _numCards(p._numCards), _numPos(p._numPos)
{
    if(TESTING)
    {
        cout << ">>Prop(cc) creating Prop object: " << this << " from Prop object: " << &p << endl;
        cout << ">>Prop(cc) _numPlayers initialized to " << _numPlayers << endl << ">>Prop(cc) _numCards initialized to " << _numCards << endl;
    }
}

Prop& Prop::operator=(const Prop& p)
{
    if(TESTING)
        cout << ">>Prop assignment operator called. Assigning: " << &p << " to: " << this << endl;
    if(&p == this)   //Check for self assignment
        return *this;
    _players = p.getPlayers();
    _weapons = p.getWeapons();
    _suspects = p.getSuspects();
    _rooms = p.getRooms();
    _cards = p.getCards();
    _numClauses = p.getNumClauses();
    _numPlayers = p.getNumPlayers();
    _numPos = _numPlayers + 1;
    _numCards = p.getNumCards();
    _numVariables = p.getNumVariables();
    return *this;
}
string Prop::addClause(vector<int>& c)
{
    ostringstream out(std::ostringstream::ate);
    ostringstream t;
    out.str(_CNF);
    for(int i = 0; i < c.size(); i++)
    {
        if(TESTING)
            t << to_string(c.at(i)) << " ";
        out << to_string(c.at(i)) << " ";
    }
    out << "0";
    if(TESTING)
    {
        t << "0";
        cout << t.str() << endl;
    }
    out << "\n";
    _CNF = out.str();
    _numClauses++;
    return _CNF;
}
string Prop::addClause(vector<string>& c)
{
    ostringstream out(std::ostringstream::ate);
    ostringstream t;
    out.str(_CNF);
    for(string str : c)
    {
        if(TESTING)
            t << str << " ";
        out << str << " ";
    }
    out << "0";
    if(TESTING)
    {
        t << "0";
        cout << t.str() << endl;
    }
    out << "\n";
    _CNF = out.str();
    _numClauses++;
    return _CNF;
}

string Prop::addTestClause(vector<string>& c)
{
    string oldCNF = _CNF;
    string temp = addClause(c);
    temp = addHead();
    _CNF = oldCNF;
    _numClauses--;
    return temp;
}
string Prop::addTestClause(vector<int>& c)
{
    if(TESTING)
        cout << "NumClauses before test clause: " << _numClauses << endl;
    string oldCNF = _CNF;
    string temp = addClause(c);
    temp = addHead();
    _CNF = oldCNF;
    if(TESTING)
        cout << "NumClauses after test clause: " << _numClauses << endl;
    _numClauses--;
    if(TESTING)
        cout << "NumClauses after _numClauses--: " << _numClauses << endl;
    return temp;
}

int Prop::toUnique(int player, int card)
{
    int h = player * _numCards + card + 1; //This generates a unique index for each card in each possible position
    return h;
}

int Prop::has(int player, int card)
{
    int h = toUnique(player, card); //This generates a unique index for each card in each possible position
    if(TESTING)
    {
        cout << ">>Prop has(" << player << "," << card << ") = " << h << endl;
    }
    return h;
}

int Prop::nHas(int player, int card)
{
    int h = -1 * toUnique(player, card); //The -1 negates the variable in CNF
    if(TESTING)
    {
        cout << ">>Prop nhas(" << player << "," << card << ") = " << h << endl;
    }
    return h;
}

string Prop::addHead()
{
    ostringstream ss(std::ostringstream::ate);
    ss << "c THIS IS THE CNF FILE FOR THE SOLVER" << endl;
    ss << "p cnf " << _numVariables << " " << _numClauses << endl;
    if(TESTING)
        cout << "Head: " << ss.str() << endl;
    ss << _CNF << endl;
    return ss.str();
}
string Prop::getCNF() const
{
    return _CNF;
}
string Prop::setCNF(string cnf)
{
    _CNF = cnf;
    return _CNF;
}
string Prop::checkSAT()
{
    if(TESTING)
        cout << ">>Prop checkSAT() called." << endl;
    string fName ="CNF.txt"; //CNF passed from Prop to Solver in this file
    string ofName ="out.txt"; //Result of solve passed from Solver to Prop in this file
    ofstream f;
    f.open(fName, ios::trunc); //ios::trunc to overwrite the file if it already exists.
    f << addHead();
    f.close();
    
    Solver solver; // create the solver
    solver.initialize(fName, ofName);  // initialize
    
    ifstream in(ofName);
    if(!in.is_open())
    {
        cout << ">>Prop(ERROR) File " << ofName << " could not be opened by the solver. TERMINATING" << endl;
        exit(1);
    }
    string line;
    if(TESTING)
        cout << ">>Prop checkSAT() output:" << endl;
    ostringstream os;
    while(getline(in, line))
    {
        os << line << endl;
    }
    if(TESTING)
        cout << os.str();
    return os.str();
    
}
int Prop::checkLiteral(int id)
{
    if(TESTING)
        cout << ">>Prop checkLiteral(" << id << ") called." << endl;
    vector<int> clause;
    clause.push_back(id);
    string CNF = addTestClause(clause);  //Add the literal to _CNF
    string fName ="CNF.txt"; //CNF passed from Prop to Solver in this file
    string ofName ="out.txt"; //Result of solve passed from Solver to Prop in this file
    ofstream f;
    f.open(fName, ios::trunc); //ios::trunc to overwrite the file if it already exists.
    f << CNF;
    f.close();
    
    Solver solver; // create the solver
    solver.initialize(fName, ofName);  // initialize
    
    ifstream in(ofName);
    if(!in.is_open())
    {
        cout << ">>Prop(ERROR) File " << ofName << " could not be opened by Prop. TERMINATING" << endl;
        exit(1);
    }
    string line;
    in >> line;
    in.close();
    return (line == "SATISFIABLE") ? 1:0;
    /*
    if(TESTING)
        cout << ">>Prop checkLiteral(" << id << ") output:" << endl;
    ostringstream os;
    while(getline(in, line))
    {
        os << line << endl;
    }
    if(TESTING)
        cout << os.str();
    return os.str();*/
}

int Prop::getNumClauses() const
{
    return _numClauses;
}

int Prop::getNumVariables() const
{
    return _numVariables;
}
int Prop::getNumPlayers() const
{
    return _numPlayers;
}
int Prop::getNumCards() const
{
    return _numCards;
}
vector<string> Prop::getSuspects() const
{
    return _suspects;
}
vector<string> Prop::getWeapons() const
{
    return _weapons;
}
vector<string> Prop::getRooms() const
{
    return _rooms;
}
vector<string> Prop::getCards() const
{
    return _cards;
}
vector<int> Prop::getPlayers() const
{
    return _players;
}
