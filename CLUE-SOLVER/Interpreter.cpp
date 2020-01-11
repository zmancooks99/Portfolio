//
//  Interpreter.cpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#include "Interpreter.hpp"
using std::string;
using std::vector;
using std::find;
using std::stoi;

Interpreter::Interpreter(std::shared_ptr<Game> gP) : _gameP(gP) { }

bool Interpreter::cardsContains(Game& g, vector<string> cards)
{
    vector<string> gameCards = g.getCards();
    if(TESTING)
    {
        cout << ">>Interpreter cardsContains checking in:" << endl;
        for(string c : gameCards)
            cout << "\t" << c << endl;
    }
    for(string card : cards)
    {
        if(find(gameCards.begin(), gameCards.end(), card) == gameCards.end())
        {
            if(TESTING)
                cout << ">>Interpreter could not interpret " << card << endl;
            return false;
        }
    }
    return true;
}
bool Interpreter::cardsContains(Game& g, string card)
{
    vector<string> gameCards = g.getCards();
    return (find(gameCards.begin(), gameCards.end(), card) != gameCards.end());
}
int Interpreter::interpret(string str)
{
    if(TESTING)
        cout << ">>Interpreter interpret received: " << str << endl << ">>Interpreter The words are:" << endl;
    
    string delimiter = " ";
    vector<string> words;
    string val;
    
    size_t pos = 0;
    while (true) //Split the string into words based on the spaces. Reliable but adds an extra trailing entry at the end of the vector
    {
        if(TESTING)
            cout << "\t\tlooping" << endl;
        if( (pos = str.find(delimiter)) == string::npos)
        {
            if(TESTING)
                cout << "\t\tFind delimiter == string::npos" << endl;
            pos = str.length();
        }
        if(pos == 0)
            break;
        val = str.substr(0, pos);
        words.push_back(val);
        str.erase(0, pos + delimiter.length());
        if(TESTING)
        {
            cout << "\t" << val << endl;
            cout << "\t(String remaining: " << str << ")" << endl;
        }
        /*if(pos == str.length())
        {
            if(TESTING)
                cout << "\t\tpos == str.length()" << endl;                      //THIS IS CREATING SPORATIC ISSUES. WHEN THE REMAINING STRING IS THE SAME LENGTH AS WHERE THE POS HAPPENED TO BE BEFORE, IT EXITS
            break;
        }*/
    }
    if(TESTING)
        cout << ">>Interpreter The number of words is: " << words.size() << endl;
    //if(words.back() == string(""))
        //words.erase(words.end()-1); //Get rid of a trailing empty entry in the vector
    
    //Set cards at beginning of game:
    if(words.at(0) == string("CARDSET"))
    {
        if(TESTING)
            cout << ">>Interpreter (CARDSET)" << endl;
        words.erase(words.begin());
        if(!cardsContains(*_gameP, words))
        {
            if(TESTING)
                cout << ">>Interpreter failed to interpret" << endl;
            return -1;
        }
        if(TESTING)
        {
            cout << ">>Interpreter setting myCards in " << _gameP->getAddress() << " to:" << endl;
            for(string c : words)
                cout << "\t" << c << endl;
        }
        _gameP->setMyCards(words);
        if(Game::TESTING)
        {
            cout << ">>Interpreter interpret() setting Game::_myCards to:" << endl;
            for(string word : words)
                cout << word << endl;
        }
        return 1;
    }
    
    //Accusation:
    if(words.at(0) == string("Accusation"))
    {
        if(TESTING)
            cout << ">>Interpreter: accusation response interpreted:" << (words.at(1) == string("yes")) << endl;
        if(words.at(1) == string("yes"))
            return 1;
        if(words.at(1) == string("no"))
            return 0;
        words.erase(words.begin());
        string valid = words.back();
        words.erase(words.end()-1);
        if(!cardsContains(*_gameP, words) || words.size() != 3)
        {
            if(TESTING)
                cout << ">>Interpreter failed to interpret" << endl;
            return -1;
        }
        if(valid == string("false"))
        {
            _gameP->falseAccusation(_gameP->getTurn(), words.at(1), words.at(2), words.at(3));
            return 0;
        }
        else if (valid == string("true"))
            return 1;
        else
        {
            if(TESTING)
                cout << ">>Interpreter failed to interpret" << endl;
            return -1;
        }
    }
    
    //Numeric first word, meaning that it is a response or a no response.
    if(words.at(0).find_first_not_of("0123456789") == string::npos)
    {
        if(TESTING)
            cout << ">>Interpreter: response interpreted." << endl;
        if(words.at(1) == string("response"))
        {
            if(TESTING)
                cout << "\tresponse" << endl;
            if(_gameP->myTurn())
            {
                if(TESTING)
                    cout << "\tmyTurn()" << endl;
                _gameP->responds(stoi(words.at(0)), words.at(2));
                return 1;
            }
            _gameP->responds(stoi(words.at(0)), currentSuggestion.at(0), currentSuggestion.at(1), currentSuggestion.at(2));
            return 1;
        }
        else if(words.at(1) == string("noresponse"))
        {
            if(TESTING)
                cout << "\tno response" << endl;
            _gameP->noResponse(stoi(words.at(0)), currentSuggestion.at(0), currentSuggestion.at(1), currentSuggestion.at(2));
            return 0;
        }
        else
        {
            if(TESTING)
                cout << ">>Interpreter failed to interpret" << endl;
            return -1;
        }
    }
    
    //First word is a card, meaning it is a suggestion
    if(cardsContains(*_gameP, words.at(0)))
    {
        if(TESTING)
            cout << ">>Interpreter: suggestion interpreted." << endl;
        if(cardsContains(*_gameP, words) && words.size() == 3)
        {
            currentSuggestion = words;
            return 1;
        }
        if(TESTING)
            cout << ">>Interpreter failed to interpret" << endl;
        return -1;
    }
    if(TESTING)
        cout << ">>Interpreter failed to interpret" << endl;
    return -1;
}
