//
//  Solver_n.hpp
//  ClueSAT
//
//  ALL CREDIT TO POSITIVEBLUE ON GITHUB FOR THEIR SIMPLESATSOLVER APP. THIS SOLVER WAS NOT WRITTEN BY ZACHARY MASON.
//  MODIFICATIONS BY ZACHARY MASON.
//

#ifndef Solver_n_hpp
#define Solver_n_hpp
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0

class Solver
{
private:
    uint numVars;
    uint numClauses;
    vector<vector<int> > clauses;
    vector<int> model;
    vector<int> modelStack;
    uint indexOfNextLitToPropagate;
    uint decisionLevel;

    //----------------------------------------------//
    //---------- Propagation ----------------------//
    vector<vector<int> > positiveOccurrences;
    vector<vector<int> > negativeOccurrences;


    //----------------------------------------------//
    //---------- Heuristic  -----------------------//
    vector<double> varScore;
    int minScore = 0;
    int maxScore = 0;
    int flag = 0;
    double payOff = 0.1;


    //----------------------------------------------//
    //---------- Output  -----------------------//
    int backtracks;
    int decisions;
    int propagations;
    void output(bool x);
    
    
    string _fIn;
    string _fOut;
public:
    ~Solver();
    Solver();
    void readClauses();
    int currentValueInModel(int lit);
    void setLiteralToTrue(int lit);
    bool propagateGivesConflict ();
    void backtrack();
    int getNextDecisionLiteral();
    void checkmodel();
    int initialize(string fInName, string fOutName);
    
    const static bool TESTING = FALSE;
};
#endif /* Solver_n_hpp */
