#pragma once
#include <vector>
#include <string>
#include <stack>
#include <unordered_set>
#include <set>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "MainFrame.h"
using namespace std;

class MainFrame;

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
	bool loopEnd;


	vector<Transition> transitions;


};





class Automata
{
public:
	
	Automata();
	Automata(string regEx);
	~Automata();

	vector<State*> states;
	bool DFA;

	void print(MainFrame* window);
	bool input(string s);
	bool createNFA(string regex);
	bool convertToDFA();
	bool minimizeDFA();
	void reset();
	void reset(string regEx);
private:
	vector<vector<State*>> miniMachines;
	unordered_set<char> alphabet;
	unordered_set<char> reservedCharacters;

	void recursiveCheck(set<pair<int, int>>& statePairs, bool* grid);
	bool testInput(string s, char currCharacter, int depth, int stringLength, State* currentState);
	
};


bool traceLambda(State* current);
void characterTransitions(char theChar, State* current, set<State*>& groupedState, int depth, bool useSteps, int steps, set<pair<int, int>> pathTaken);
int getNextStateID(char theChar, State* current);
