//
//  main.cpp
//  ClueSAT
//
//  Created by Zachary Mason.
//  Copyright © 2019 Zachary Mason. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "Game.hpp"
#include "Interpreter.hpp"
#include "Solver_n.hpp"
#include "Prop.hpp"

using std::cout;
using std::endl;
using std::cin;
using std::shared_ptr;
using std::make_shared;
using std::setw;

//  checkError(Interpreter object, int of result of attempt to interpret, int of active player number)
//
//  Checks if there's an error in the response from the user.
//  Does not throw
//  Exception Neutral
int checkError(Interpreter i, int result, int activeP)
{
    while(result < 0)
    {
        string response;
        cout << "Error interpreting response. Please reenter." << endl;
        getline(cin, response);
        if(activeP >= 0)
            response = to_string(activeP) + " " + response;
        result = i.interpret(response);
        response = "";
    }
    return result;
}

//Main function

//Runs game
int main(int argc, const char * argv[]) {
    if(!(Game::TESTING && Prop::TESTING && Solver::TESTING && Interpreter::TESTING))
    {
        cout << "TESTING ONLY PARTIALLY ENABLED Game::TESTING=" << Game::TESTING << "\tProp::TESTING=" << Prop::TESTING << "\tSolver::TESTING=" << Solver::TESTING << "\tInterpreter::TESTING=" << Interpreter::TESTING << endl;
    }
    int numP = 0;
    cout << "How many players?" << endl;
    cin >> numP;
    while(numP < 3 || numP > 6)
    {
        cout << "Please enter between 3 and 6 players." << endl;
        cin >> numP;
    }
    cout << "Which player are you, assuming play starts with player 1?" << endl;
    int playNum;
    cin >> playNum;
    while(playNum > numP || playNum < 1)
    {
        cout << "Please enter a valid player number." << endl;
        cin >> playNum;
    }
    
    /*if(Game::TESTING)
    {
        numP = 5;
        playNum = 0;
    }*/
    shared_ptr<Game> gameP = make_shared<Game>(numP, playNum-1); //-1 to prevent off by one error since indexes start at 0
    Interpreter i {gameP};
    string cards = "";
    cout << "Which cards do you have? (separated by spaces. No caps, dots, or spaces in card names)" << endl;
    cin.get();  //Skip the trailing newline from the last cin
    getline(cin, cards);
    while(cards == "")
    {
        cout << "Please provide a non-empty response. Which cards do you have?" << endl;
        getline(cin, cards);
    }
    if(Game::TESTING)
    {
        cout << ">>Main received input " << cards << " [Press enter to continue] " << endl;
        cin.get();
    }
    cards = "CARDSET " + cards;
    i.interpret(cards); //This needs to call Game.setMyCards(vector<string>) and pass the cards as a vector.
    /*if(Game::TESTING)
    {
        vector<string> cardsv = {"hall", "diningroom","mrgreen","knife"};
        game.setMyCards(cardsv);
    }*/
    Prop _prop{gameP->getPlayers(), gameP->getSuspects(), gameP->getWeapons(), gameP->getRooms(), gameP->getCards()};
    gameP->setProp(_prop);
    gameP->defaultAssumptions();
    cout << "You're ready to go! Remember that everytime you enter cards, it should be in the order suspect, weapon, room." << endl << endl;;
    while(true)
    {
        string response;
        int result;
        if(!gameP->myTurn())
        {
            cout << "It is player " << gameP->getTurn()+1 << "'s turn." << endl;
            cout << "What suggestion do they make?" << endl;
        }
        else
        {
            cout << "It is your turn." << endl;
            cout << "What suggestion do you make?" << endl;
        }
        getline(cin, response);
        response = response;
        result = i.interpret(response);
        response = "";
        checkError(i, result, -1);
        int activeP = gameP->getTurn()+1;
        cout << "How does player " << activeP+1 << " respond? (\"response [card]\" or \"noresponse\")" << endl;
        getline(cin, response);
        response = to_string(activeP) + " " + response;
        result = i.interpret(response);
        response = "";
        checkError(i, result, activeP);
        while(result == 0)  //A no response. Play passes until the next player until someone responds.
        {
            cout << "How does player " << ++activeP+1 << " respond? (\"response [card]\" or \"noresponse\")" << endl;
            getline(cin, response);
            response = to_string(activeP) + " " + response;
            result = i.interpret(response);
            response = "";
        }
        cout << "Does player " << gameP->getTurn()+1 << " make an accusation?" << endl;
        getline(cin, response);
        response = "Accusation " + response;
        result = i.interpret(response); //Return 1 if yes, 0 if no
        response = "";
        checkError(i, result, -1);
        if(result)
        {
            cout << "Enter the accusation along with \"true\" or \"false\" to indicate if it was correct." << endl;
            getline(cin, response);
            response = "Accusation " + response;
            result = i.interpret(response); //Return 1 if accusation correct. 0 if incorrect.
            checkError(i, result, -1);
            if(result)
            {
                cout << "Game over, someone made a correct accusation." << endl;
                exit(0);
            }
        }
        if(Game::TESTING)
        {
            gameP->getProp().checkSAT();
            ifstream f;
            f.open("out.txt");
            string output;
            f >> output;
            if(output == string("UNSATISFIABLE"))
            {
                cout << "UNSATISFIABLE" << endl;
                throw std::runtime_error("UNSATISFIABLE BOARD");
            }
            cout << "CURRENTLY STILL SATISFIABLE [enter to continue]" << endl;
            cin.get();
        }
        //Print out the notepad
        if(Game::TESTING)
        {
            int numV = gameP->getProp().getNumVariables();
            cout << ">>Main There should be " << numV << " vars. [enter to continue]" << endl;
            cin.get();
            /*int c = 0;
            for(int k = 2; k <= numV; k++)
            {
                cout << ">>Main \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << k << endl;
                for(int i = 20; i < 20+k ; i++, c++)
                {
                    gameP->getProp().checkLiteral(i);
                    cout << c << endl;
                }
            }*/
            /*for(int i = 1; i <= numV ; i++)
            {
                gameP->getProp().checkLiteral(11);
                cout << i << endl;
            }*/
        }
                      //COMMENTED FOR TESTING
        int numLit = 0;             //For testing. The number of literals that have been checked so far
        cout << "\t\t\t\t\t-----NOTEPAD-----" << endl;
        cout << "\t\t\t\t\t'X' means they may have the card." << endl;
        const int strW = 13;
        int numPlayers = gameP->getNumPlayers() ;
        
        cout << setw(strW) << "Players:";
        for(int i = 0; i < numPlayers+1; i++) //using numplayers+1 to include the mystery card position
            cout << setw(strW-1) << i+1 << ":"; //Output the players themselves
        cout << endl;
        
        for(int i = 0; i <= numPlayers+2; i++)          //Just to make the separating line long enough -- just for pretty stuff
            cout << setw(strW) << "_____________";
        cout << endl;
        
        for(string c : gameP->getCards())
        {
            cout << setw(strW) << c << ":";
            for(int i = 0; i < numPlayers+1; i++, numLit++)
            {
                int cardI =gameP->getCardIndex(c);
                int u = gameP->getProp().toUnique(i, cardI);
                cout << setw(strW) << ( (gameP->getProp().checkLiteral(u)) ? "X":"O"); //Check the literal and put it to cout
                if(Game::TESTING)
                    cout << ">>Main \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t tested " << numLit << " literals" << endl;
            }
            cout << endl << endl;
        }
        /*
        int nv = gameP->getNumVariables();
        int nc = gameP->getNumCards();
        int np = numP;
        for(int i = 1; i <= nv; i++) //nv is the number of variables in the program.
        {
            cout << gameP->checkLiteral(i) << endl;
        }*/
         
        gameP->pass();
        
    }
    
    if(Game::TESTING)
    {
        cout << ">>Main numClauses = " << gameP->getProp().getNumClauses() << endl;
        cout << ">>Main REPRINT FULL CNF?(y,n)" << endl;
        char c;
        cin >> c;
        if(c == 'y')
        {
            cout << gameP->getProp().addHead() << endl;
        }
        gameP->getProp().checkSAT();
        //game.getProp().checkLiteral(game.getProp().has(game.getProp().getNumPlayers(), game.getCardIndex("hall"))); //test literal
        //game.getProp().checkLiteral(game.getProp().has(game.getProp().getNumPlayers(), game.getCardIndex("candlestick")));
        Solver solver;
        solver.initialize("TESTCNF.txt", "TESTout.txt");
    }
    return 0;
}
