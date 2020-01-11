//
//  Solver_n.cpp
//  ClueSAT
//
//  ALL CREDIT TO POSITIVEBLUE ON GITHUB FOR THEIR SIMPLESATSOLVER APP. THIS SOLVER WAS NOT WRITTEN BY ZACHARY MASON.
//  MODIFICATIONS BY ZACHARY MASON.
//

#include "Solver_n.hpp"

Solver::Solver()
{
    if(TESTING)
        cout << ">>Solver(c) created: " << this << endl;
}
Solver::~Solver()
{
    if(TESTING)
        cout << ">>Solver(d) being destroyed: " << this << endl;
}
void Solver::output(bool x){
    ofstream of(_fOut, ios::trunc);
    if (!x) of << "UNSATISFIABLE ";
    else of << "SATISFIABLE ";
    of << decisions << " " << propagations << " " << backtracks << endl;
    of.close();
}

void Solver::readClauses(){
    ifstream f;
    f.open(_fIn);
    
    // Skip comments
    char c = f.get();
    while (c == 'c') {
        while (c != '\n') c = f.get();
        c = f.get();
    }
    // Read "cnf numVars numClauses"
    string aux;
    f >> aux >> numVars >> numClauses;
    if(TESTING)
        cout << ">>Solver parsed " << numVars << " vars and " << numClauses << " clauses." << endl;
    clauses.resize(numClauses);
    
    positiveOccurrences.resize(numVars+1);
    negativeOccurrences.resize(numVars+1);
    
    varScore.resize(numVars+1,0);
    flag = numVars;
    
    backtracks = decisions = propagations = 0;
    // Read clauses
    for (uint i = 0; i < numClauses; ++i) {
        int lit;
        while (f >> lit and lit != 0){
            clauses[i].push_back(lit);
            lit > 0 ? positiveOccurrences[lit].push_back(i) : negativeOccurrences[-lit].push_back(i);
            ++varScore[abs(lit)];
        }
    }
    f.close();
}



int Solver::currentValueInModel(int lit){
    if (lit >= 0) return model[lit];
    else {
        if (model[-lit] == UNDEF) return UNDEF;
        else return 1 - model[-lit];
    }
}


void Solver::setLiteralToTrue(int lit){
    modelStack.push_back(lit);
    if (lit > 0) model[lit] = TRUE;
    else model[-lit] = FALSE;
}


bool Solver::propagateGivesConflict ( )
{
    while ( indexOfNextLitToPropagate < modelStack.size() )
    {
        int lit = modelStack[indexOfNextLitToPropagate];
        ++indexOfNextLitToPropagate;
        vector<int>* whereToLook = lit > 0 ? &negativeOccurrences[lit] : &positiveOccurrences[-lit];
        for (uint i = 0; i < whereToLook->size(); ++i)
        {
            bool someLitTrue = false;
            int numUndefs = 0;
            int lastLitUndef = 0;
            int w = (*whereToLook)[i];
            long s = clauses[w].size();
            for (uint k = 0; not someLitTrue and k < s; ++k)
            {
                int val = currentValueInModel(clauses[(*whereToLook)[i]][k]);
                if (val == TRUE)
                    someLitTrue = true;
                else if (val == UNDEF)
                {
                    ++numUndefs;
                    lastLitUndef = clauses[(*whereToLook)[i]][k];
                }
            }
            if (not someLitTrue and numUndefs == 0)
            {
                ++backtracks;
                --flag;
                if (flag == 0)
                {
                    flag = numVars;
                    payOff+=0.1;
                }
                varScore[abs(lit)]+= payOff;
                
                return true; // conflict! all lits false
            }
            else if (not someLitTrue and numUndefs == 1)
            {
                ++propagations;
                setLiteralToTrue(lastLitUndef);
            }
        }
    }
    return false;
}


void Solver::backtrack(){
    uint i = modelStack.size() -1;
    int lit = 0;
    while (modelStack[i] != 0){ // 0 is the DL mark
        lit = modelStack[i];
        model[abs(lit)] = UNDEF;
        modelStack.pop_back();
        --i;
    }
    // at this point, lit is the last decision
    modelStack.pop_back(); // remove the DL mark
    --decisionLevel;
    indexOfNextLitToPropagate = modelStack.size();
    setLiteralToTrue(-lit);  // reverse last decision
}


// Heuristic for finding the next decision literal:
int Solver::getNextDecisionLiteral(){
    ++decisions;
    int max = -1;
    int lit = -1;
    
    for (int i = 1; i <= varScore.size(); ++i){
        if (model[i] == UNDEF and varScore[i] > max) {
            max = varScore[i];
            lit = i;
        }
    }
    if (lit > 0) return lit;
    return 0;
    
    //for (uint i = 1; i <= numVars; ++i) // stupid heuristic:
    //  if (model[i] == UNDEF) return i;  // returns first UNDEF var, positively
    //return 0; // reurns 0 when all literals are defined
}

void Solver::checkmodel(){
    for (int i = 0; i < numClauses; ++i){
        bool someTrue = false;
        for (int j = 0; not someTrue and j < clauses[i].size(); ++j)
            someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
        if (not someTrue) {
            ofstream of(_fOut, ios::trunc);
            of << "Error in model, clause is not satisfied:";
            for (int j = 0; j < clauses[i].size(); ++j) of << clauses[i][j] << " ";
            of << endl;
            of.close();
        }
    }
}

int Solver::initialize(string fInName, string fOutName){
    _fOut = fOutName;
    _fIn = fInName;
    srand (time(NULL));
    readClauses(); // reads numVars, numClauses and clauses
    model.resize(numVars+1,UNDEF);
    indexOfNextLitToPropagate = 0;
    decisionLevel = 0;
    
    // Take care of initial unit clauses, if any
    for (uint i = 0; i < numClauses; ++i)
        if (clauses[i].size() == 1) {
            int lit = clauses[i][0];
            int val = currentValueInModel(lit);
            if (val == FALSE) {output(false); return 10;}
            else if (val == UNDEF) setLiteralToTrue(lit);
        }
    
    // DPLL algorithm
    while (true) {
        while ( propagateGivesConflict() ) {
            if ( decisionLevel == 0) { output(false); return 10; }
            backtrack();
        }
        int decisionLit = getNextDecisionLiteral();
        if (decisionLit == 0) { checkmodel(); output(true); return 20; }
        // start new decision level:
        modelStack.push_back(0);  // push mark indicating new DL
        ++indexOfNextLitToPropagate;
        ++decisionLevel;
        setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
    }
}
