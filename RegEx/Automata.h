#pragma once
#include <vector>
#include <string>
#include <stack>
#include <unordered_set>
#include <set>
using namespace std;


struct State;

/*
A transition is a 1-Directional pointer to a State, with a specified character input necessary for transition
*/
struct Transition
{
	Transition();
	Transition(char acceptedChar);
	Transition(char acceptedChar, State* nextState);


	char acceptedChar;
	State* nextState;


};


/*
A state has an identifier and zero or more transitions

a state may also be marked as a final state
*/
struct State
{
	State();
	State(bool fnState);
	
	static int stateCounter;
	
	int id;
	bool finalState;
	bool initialState;
	bool loopStart;


	vector<Transition> transitions;


};





class Automata
{
public:
	
	Automata();
	Automata(string regEx);
	Automata(char, bool testingPurposes);
	~Automata();

	vector<State*> states;
	bool DFA;

	void print();
	void input(string s);
	void convertToDFA();
	void minimizeDFA();
	void reset();
	void reset(string regEx);
private:
	vector<vector<State*>> miniMachines;
	stack<State*> nestedRoot;
	stack<State*> starClosures;
	unordered_set<char> alphabet;
	void recursiveCheck(set<pair<int, int>>& statePairs, bool* grid);
	void createNFA(string regEx, char currCharacter, int depth, int stringLength, State* currentState, bool nested, bool star, bool endOfString);
	void createNFA2(string regex);
	bool testInput(string s, char currCharacter, int depth, int stringLength, State* currentState);
	void traverseAndSetNext(State* current, State* next, char theChar, bool star);
};


bool traceLambda(State* current);
void characterTransitions(char theChar, State* current, set<State*>& groupedState, int depth, bool useSteps, int steps, set<pair<int, int>> pathTaken);
int getNextStateID(char theChar, State* current);