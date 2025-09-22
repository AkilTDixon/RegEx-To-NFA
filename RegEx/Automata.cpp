#define GVDLL
#include "Automata.h"
#include <iostream>
#include <set>
#include <unordered_set>
#include <utility>
#include <stack>
#include <fstream>
#include <map>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <cstdio>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
using namespace std;



int State::stateCounter = -1;

Transition::Transition()
{

}

Transition::Transition(char c)
{
	acceptedChar = c;
	nextState = nullptr;
}

Transition::Transition(char c, State* n)
{
	acceptedChar = c;
	nextState = n;
}



State::State()
{
	stateCounter++;
	id = stateCounter;
	finalState = false;
	loopStart = false;
	loopEnd = false;
	initialState = false;
	
}
State::State(bool fnState)
{
	stateCounter++;
	id = stateCounter;
	finalState = fnState;
	loopStart = false;
	loopEnd = false;
	initialState = false;
}


Automata::Automata()
{
	reservedCharacters.insert('(');
	reservedCharacters.insert(')');
	reservedCharacters.insert('*');
	reservedCharacters.insert('+');
	DFA = false;
	

}

Automata::Automata(string regEx)
{

	reservedCharacters.insert('(');
	reservedCharacters.insert(')');
	reservedCharacters.insert('*');
	reservedCharacters.insert('+');

	DFA = false;

	

	createNFA(regEx);

	
}


Automata::~Automata()
{
	reset();
}

void Automata::reset()
{
	for (auto s : states) {
		delete s;
		s = nullptr;
	}

	states.clear();
	alphabet.clear();
	DFA = false;
	for (auto m : miniMachines) {
		for (auto n : m)
		{
			delete n;
			n = nullptr;
		}
		m.clear();
	}
	miniMachines.clear();
	

	State::stateCounter = -1;

}

void Automata::reset(string regEx)
{
	for (auto s : states) {
		delete s;
		s = nullptr;
	}

	states.clear();
	alphabet.clear();
	DFA = false;
	for (auto m : miniMachines) {
		for (auto n : m)
		{
			delete n;
			n = nullptr;
		}
		m.clear();
	}
	miniMachines.clear();


	State::stateCounter = -1;


	createNFA(regEx);

}

bool Automata::input(string s)
{
	int depth = 0;

	

	return testInput(s, s[depth], depth, s.length(), states[0]);
}


void Automata::print(MainFrame* window)
{
	map<int, string> transitionOn;
	map<int, Agedge_t*> idToEdge;
	vector<Agnode_t*> allNodes;
	string label;
	GVC_t* gvc = gvContext();

	//directed graph
	Agraph_t* g = agopen((char*)"G", Agdirected, 0);
	agsafeset(g, (char*)"rankdir", (char*)"LR", (char*)"");


	//Nodes
	for (auto s : states){
		
		Agnode_t* n = agnode(g, (char*)(to_string(s->id)).c_str(), 1);
		
		if (s->finalState)
		{
			agsafeset(n, (char*)"style", (char*)"filled", (char*)"");
			agsafeset(n, (char*)"fillcolor", (char*)"green", (char*)"");
		}
		allNodes.push_back(n);
	}

	//Edges
	for (int i = 0; i < states.size(); i++){
		for (auto t : states[i]->transitions)
		{
			string toSfromC(1, t.acceptedChar);
			if (!transitionOn.contains(t.nextState->id))
				transitionOn[t.nextState->id] = toSfromC;
			else if (transitionOn[t.nextState->id] != toSfromC)
			{	
				transitionOn[t.nextState->id] += ", " + toSfromC;
				agdelete(g, idToEdge[t.nextState->id]);
				
			}
			label = transitionOn[t.nextState->id];


			
			Agedge_t* e = agedge(g, allNodes[i], allNodes[t.nextState->id],0, 1);
			idToEdge[t.nextState->id] = e;
			//agsafeset(e, (char*)"label", (char*)(toSfromC).c_str(), (char*)"");
			agsafeset(e, (char*)"label", (char*)(char*)(label).c_str(), (char*)"");
			agsafeset(e, (char*)"labeldistance", (char*)"500.0", (char*)"");
			agsafeset(e, (char*)"labelangle", (char*)"30", (char*)"");
		}
		transitionOn.clear();
	}
	gvLayout(gvc, g, "dot");
	
	gvRenderFilename(gvc, g, "png", "output.png");

	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);

	wxImage image;
	if (image.LoadFile("output.png"))
	{
		int maxWidth = 800, maxHeight = 450;
		double scaleX = (double)maxWidth / image.GetWidth(), scaleY = (double)maxHeight / image.GetHeight();
		double scale = min(scaleX, scaleY);
		if (scale < 1.0)
			image = image.Scale(image.GetWidth() * scale, image.GetHeight() * scale, wxIMAGE_QUALITY_HIGH);

		wxBitmap bitmap(image);
		int x = (window->GetSize().GetWidth() - bitmap.GetWidth()) / 2;
		//int y = (windowPanel->GetSize().GetHeight() - bitmap.GetHeight()) / 2;
		window->staticBitmap = new wxStaticBitmap(window->windowPanel, wxID_ANY, bitmap, wxPoint(x, 10));

	}


}

bool Automata::testInput(string s, char currCharacter, int depth, int stringLength, State* currentState)
{
	
	if (depth >= stringLength)
	{	
		//check if there is any path of lambda transitions where it ends in a final state
		bool ret = traceLambda(currentState);

		//does it end inside a final state?
		return ret || currentState->finalState;
	}

	bool ret = false;
	
	if (currentState->transitions.size() > 0)
	{
		for (int i = 0; i < currentState->transitions.size(); i++)
		{
			Transition temp = currentState->transitions[i];
			
			if (currCharacter == temp.acceptedChar || temp.acceptedChar == '~')
			{
				//if using a lambda transition (~), don't eat the input. meaning don't increment the depth
				if (temp.acceptedChar == '~' && currentState->transitions[i].nextState == currentState)
					continue;
				
				if (temp.acceptedChar != '~')
					depth++;
				
				ret = ret || testInput(s, s[depth], depth, stringLength, temp.nextState);
				if (ret)
					return true;

				if (temp.acceptedChar != '~')
					depth--;
			}
		}
	}

	return ret;

}
bool traceLambda(State* current)
{
	bool ret = false;

	//accept input on the first lambda transition that leads to a final state
	if (current->finalState)
		return true;

	if (current->transitions.size() == 0)
		return current->finalState;
	else if (current->transitions.size() > 0)
	{
		for (int i = 0; i < current->transitions.size(); i++)
		{
			
			if (current->transitions[i].acceptedChar == '~') {
				if (current->transitions[i].nextState == current)
					continue;
				ret = ret || traceLambda(current->transitions[i].nextState);
				if (ret)
					return true;
			}
		}
	}
	return ret;
}
void Automata::createNFA(string regex)
{

	char currCharacter;
	bool branching = false;
	bool bracket = false;
	bool bracketEnd = false;
	bool star = false;
	bool branchingBracket = false;
	stack<int> bracketCount;
	int rootMachine = -1;
	vector<State*> mini, starMini;

	//trying to follow a more algorithmic approach
	for (int i = 0; i < regex.length(); i++)
	{

		currCharacter = regex[i];
		//making mini NFAs for each literal
		if (!reservedCharacters.contains(currCharacter))
			if (!alphabet.contains(currCharacter))
				alphabet.insert(currCharacter);
	
		if (currCharacter != '(' && currCharacter != ')')
		{
			if (currCharacter != '*' && currCharacter != '+')
			{
				State* newS = new State();
				State* next = new State();
				next->finalState = true;
				Transition t(currCharacter, next);
				newS->transitions.push_back(t);
				if (miniMachines.empty() && mini.empty())
					newS->initialState = true;
				else
				{
					if (branching)
					{
						Transition e('~', newS);
						if (branchingBracket)
						{
							miniMachines[rootMachine][0]->transitions.push_back(e);
							branchingBracket = false;
						}
						else if (!bracketCount.empty())
						{
							int index = bracketCount.top();
							//the initial state of the root machine needs an epsilon transition to this machine
							if (index >= miniMachines.size())
								index = miniMachines.size() - 1;
							miniMachines[index][0]->transitions.push_back(e);
							
						}
						else
						{
							if (rootMachine != -1)
							{
								miniMachines[rootMachine][0]->transitions.push_back(e);
								rootMachine = -1;			
							}
							else
							{
								//the previous minimachine's initial state needs to have an epsilon transition to this new machine
								int index = miniMachines.size() - 1;
								miniMachines[index][0]->transitions.push_back(e);
							}
						}
						branching = false;
					}
					else
					{
						
						if (bracketEnd)
						{
							//all the final states in the bracket need to connect with this state (concatenation)
							int index = rootMachine;
							for (int k = index; k < miniMachines.size(); k++)
							{
								for (int l = 0; l < miniMachines[k].size(); l++)
								{
									if (miniMachines[k][l]->finalState)
									{
										Transition e('~', newS);
										miniMachines[k][l]->transitions.push_back(e);
										miniMachines[k][l]->finalState = false;
									}
								}
							}
							//bracketCount.pop();
							bracketEnd = false;
							if (bracketCount.empty())
								rootMachine = -1;
						}
						else
						{
							//this means all the final states of the last machine must epsilon transition to this initial state
							int index = miniMachines.size() - 1;
							for (int k = 0; k < miniMachines[index].size(); k++)
							{
								if (miniMachines[index][k]->finalState)
								{
									Transition e('~', newS);
									miniMachines[index][k]->transitions.push_back(e);
									miniMachines[index][k]->finalState = false;
								}

							}
						}
					}				
				}
				mini.push_back(newS);
				mini.push_back(next);
					
			}
			else if (currCharacter == '*')
			{
				/*
				When star closure is called, it should create a new intermediary node. Every machine closed under the star should have
				its final states epsilon transition to the new node
				That new node should then epsilon transition to a new "initial state" for the machine, and anything that transitions to
				the old "initial state" should now transition to the new one
				
				*/

				State* intermediary = new State();
				intermediary->loopEnd = true;
				State* newInitial = new State();

				int startAtMachine;
				
				if (rootMachine != -1)
				{
					startAtMachine = rootMachine;
					
					//the star closed machines should have their final states epsilon transition to the intermediary
					for (int k = startAtMachine; k < miniMachines.size(); k++)
					{
						for (int l = 0; l < miniMachines[k].size(); l++)
						{
							if (miniMachines[k][l]->finalState)
							{
								Transition e('~', intermediary);
								miniMachines[k][l]->transitions.push_back(e);
								miniMachines[k][l]->finalState = false;
							}
						}
					}
					
				}
				else
				{

					//then the previously saved minimachine must be looped
					//by putting an epsilon transition to its initial state from its final state
					startAtMachine = miniMachines.size() - 1;
					int innerIndex = miniMachines[startAtMachine].size() - 1;
					Transition t('~', intermediary);

					miniMachines[startAtMachine][innerIndex]->transitions.push_back(t);
					if (miniMachines[startAtMachine][innerIndex]->finalState)
						miniMachines[startAtMachine][innerIndex]->finalState = false;
					
				}

				auto oldInitial = miniMachines[startAtMachine][0];
				//the intermediary should epsilon transition to the newInitial
				Transition e('~', newInitial);
				intermediary->transitions.push_back(e);
				e.nextState = oldInitial;
				//the newInitial should epsilon transition to the current start node for this miniMachine
				newInitial->transitions.push_back(e);
				newInitial->finalState = true;
				if (oldInitial->initialState)
				{
					oldInitial->initialState = false;
					newInitial->initialState = true;
				}
				
			
				
				/*
				Anything that was transitioning to the old initial state needs to transition to the new one
				*/
				for (int m = 0; m < miniMachines.size(); m++)
				{
					for (int x = 0; x < miniMachines[m].size(); x++)
					{
						for (int y = 0; y < miniMachines[m][x]->transitions.size(); y++)
						{
							if (miniMachines[m][x]->loopEnd)
								break;
							if (miniMachines[m][x]->transitions[y].nextState == oldInitial)
								miniMachines[m][x]->transitions[y].nextState = newInitial;
						}
					}
				}
				int temp = oldInitial->id;

				oldInitial->id = newInitial->id;
				newInitial->id = temp;
				miniMachines[startAtMachine].push_back(newInitial);
				swap(miniMachines[startAtMachine][0], miniMachines[startAtMachine][miniMachines[startAtMachine].size() - 1]);
				miniMachines[startAtMachine].push_back(intermediary);


			}
			else if (currCharacter == '+')
			{
				//then the previously saved minimachine's initial state must branch into this next machine
				branching = true;
				bracketEnd = false;

				/*
				when doing an OR, create a new state. This state will epsilon transition to the rootMachine of the first half,
				and epsilon transition to the next half, becoming the new branching initial state
				*/

				State* newS = new State();
				int theMachine = rootMachine;
				
				if (bracketCount.empty() && rootMachine < 0)
					theMachine = 0;
				else if (!bracketCount.empty())
					theMachine = bracketCount.top();
				
				auto oldInitial = miniMachines[theMachine][0];
				Transition e('~', miniMachines[theMachine][0]);
				newS->transitions.push_back(e);
				if (miniMachines[theMachine][0]->initialState)
				{
					newS->initialState = true;
					miniMachines[theMachine][0]->initialState = false;
				}

				//anything that was transitioning to the old initial/entry state now needs to transition to the
				//new state
				for (int m = 0; m < miniMachines.size(); m++)
				{
					for (int x = 0; x < miniMachines[m].size(); x++)
					{
						for (int y = 0; y < miniMachines[m][x]->transitions.size(); y++)
						{
							if (miniMachines[m][x]->loopEnd)
								break;
							if (miniMachines[m][x]->transitions[y].nextState == oldInitial)
								miniMachines[m][x]->transitions[y].nextState = newS;
						}
					}
				}


				miniMachines[theMachine].push_back(newS);
				swap(miniMachines[theMachine][0], miniMachines[theMachine][miniMachines[theMachine].size() - 1]);

				
			}
		}
		else if (currCharacter == ')')
		{
			bracketEnd = true;
			rootMachine = bracketCount.top();
			bracketCount.pop();
		}
		else
		{
			//brackets. whatever is inside the brackets will be its own machine
			//for now its just basically concatenation
			bracket = true;
			
			if (branching)
			{
				if (!branchingBracket)
				{
					branchingBracket = true;
					//not in a bracket
					if (!bracketCount.empty())
						rootMachine = bracketCount.top();
					else
						rootMachine = 0;
				}
			}
			

			bracketCount.push(miniMachines.size());
			//the first machine inside the bracket will be the next created one
			
		}
		if (!mini.empty())
			miniMachines.push_back(mini);

		mini.clear();

	}

	if (miniMachines.size() < 1)
	{
		//meaning it's an empty string with one or more blank spaces
		State* newS = new State();
		State* next = new State();
		next->finalState = true;
		Transition t('~', next);
		newS->transitions.push_back(t);
		newS->initialState = true;
		mini.push_back(newS);
		mini.push_back(next);
		miniMachines.push_back(mini);

		mini.clear();
	}

	for (int i = 0; i < miniMachines.size(); i++)
	{
		for (int j = 0; j < miniMachines[i].size(); j++)
			states.push_back(miniMachines[i][j]);
		
		miniMachines[i].clear();
	}

	miniMachines.clear();

	for (int i = 0; i < states.size(); i++)
		states[i]->id = i;

}




void Automata::convertToDFA()
{
	if (states.size() < 1)
		return;
	vector<set<State*>> p;
	set<set<State*>> masterList;
	set<State*> groupedState;
	set<pair<int, int>> pathTaken;

	
	groupedState.insert(states[0]);

	/*
	Starting with the initial state, create a new grouped state that's the result of any 
	and all epsilon transitions from q0

	the initial state of the new DFA will be the result of the epsilon transition on q0
	*/
	characterTransitions('~', states[0], groupedState, 0, false, 0, pathTaken);
	if (!masterList.contains(groupedState))
	{
		p.push_back(groupedState);
		masterList.insert(groupedState);
	}
	groupedState.clear();
	/*
	groupedState will now contain all states that are reachable from q0 by just epsilon transitions
	{q0, q1}
	
	after getting this, we need to see what transitions this combined state could have

	for every state inside the groupedState, we test to see if they have any transitions according to our alphabet
	so, does q0 have a transition on 'a', if so where does it go, add that state to a new groupedState
	*/
	

	//from index, to index, on character
	//indices correspond to the vector p
	vector<pair<pair<int, int>, char>> allTransitions;
	for (auto l = 0; l < p.size(); l++)
	{
		for (auto k = alphabet.begin(); k != alphabet.end(); k++)
		{
			for (auto i = p[l].begin(); i != p[l].end(); i++)
				characterTransitions(*k, *i, groupedState, 0, true, 1, pathTaken);
			
			for (auto i = groupedState.begin(); i != groupedState.end(); i++)
			{
				/*
				with the new groupedState, compute the epsilon closure of the states inside it
				*/
				characterTransitions('~', *i, groupedState, 0, false, 0, pathTaken);
			}
			if (!groupedState.empty())
			{
				if (!masterList.contains(groupedState))
				{
					p.push_back(groupedState);
					masterList.insert(groupedState);
				}
			}
			groupedState.clear();

		}
	}
	
	/*
	With all the possible groupings of states generated, now we need to connect the states with transitions
	We need to iterate through the list of the sets, and for each state, if the state1 has a transition to
	state2 on 'a', then the whole set has a transition of 'a' to any set that contains that state
	*/

	
	/*
	I'll need to keep track of which states are final states
	*/

	
	
	for (int i = 0; i < p.size(); i++)
	{
		for (auto k = alphabet.begin(); k != alphabet.end(); k++)
		{
			for (auto l = p[i].begin(); l != p[i].end(); l++)
				characterTransitions(*k, *l, groupedState, 0, true, 1, pathTaken);
			
			for (auto l = groupedState.begin(); l != groupedState.end(); l++)
			{
				/*
				with the new groupedState, compute the epsilon closure of the states inside it
				*/
				characterTransitions('~', *l, groupedState, 0, false, 0, pathTaken);
			}
			if (!groupedState.empty())
			{
				if (auto it = find(p.begin(), p.end(), groupedState); it != p.end())
					allTransitions.push_back({{i, distance(p.begin(), it)}, *k});
			}
			else
			{
				//if the state goes nowhere on a character, then on that character make a transition to
				//an empty trap state, indicated by p.size()
				allTransitions.push_back({ {i, p.size()}, *k });
			}
			groupedState.clear();
		}
	}

	/*
	With all the transitions generated, a state machine can now be made based on the gathered data
	The number of states is constrained by the number of elements in the p vector

	I could loop and create the exact number of states necessary, and then apply the transitions to them
	*/
	int numOfStates = p.size();
	bool* fnStates = new bool[numOfStates+1]();
	int initialStateID = 0;

	for (int i = 0; i < numOfStates; i++)
	{
		auto currentSet = p[i];
		for (auto k = currentSet.begin(); k != currentSet.end(); k++)
		{
			if ((*k)->finalState)
			{
				fnStates[i] = true;
				break;
			}

		}
	}
	masterList.clear();
	p.clear();
	for (auto s : states)
		delete s;

	states.clear();
	State::stateCounter = -1;

	//states[numOfStates] = trap state
	
	for (int i = 0; i < numOfStates+1; i++)
	{
		State* newState = new State();
		newState->finalState = fnStates[i];	
		if (i == 0)
			newState->initialState = true;
		states.push_back(newState);		
	}
	//ignore making a trap state for now
	for (auto k = alphabet.begin(); k != alphabet.end(); k++)
	{
		Transition trap(*k, states[numOfStates]);
		states[numOfStates]->transitions.push_back(trap);
	}

	int from, to;
	char theChar;
	for (int i = 0; i < allTransitions.size(); i++)
	{

		from = allTransitions[i].first.first;
		to = allTransitions[i].first.second;
		theChar = allTransitions[i].second;

		
		Transition t(theChar, states[to]);
		states[from]->transitions.push_back(t);
	}
	for (int i = 0; i < states.size(); i++)
		if (states[i]->initialState && i != 0) {
			swap(states[0], states[i]);
			break;
		}
	for (int i = 0; i < states.size(); i++)
		states[i]->id = i;
	
	State::stateCounter = states.size()-1;
	DFA = true;
	delete[] fnStates;
}
void Automata::minimizeDFA()
{
	if (!DFA)
		return;

	set<pair<int, int>> statePairs;

	/*
	Distinguishable states:
	p,q
	1. p is a final state, q is NOT a final state
	2. p is NOT a final state, q is a final state
	3. T(p, a), T(q, a) are both distinguishable, meaning
		the transition from p, on some character 'a', and the same for q, both lead to states that are
		distinguishable from each other (per the 1, 2, 3)


	Instead of making a table, just isolate each individual pair, so that there isn't too much unnecessary data
	
	*/
	
	for (int i = 0; i < states.size(); i++)
	{
		for (int j = 0; j < states.size(); j++)
		{
			if (j == i)
				continue;
			if (!statePairs.contains({i, j}) && !statePairs.contains({ j, i }))
				statePairs.insert({i, j});
		}
	}

	/*
	With every unique pair, we associate distinguishability
	*/
	int id1, id2;
	bool* grid = new bool[statePairs.size()];
	
	//false for not indistinguishable, true for distinguishable
	//determine final state status
	for (auto i = statePairs.begin(); i != statePairs.end(); i++)
	{
		id1 = (*i).first;
		id2 = (*i).second;

		if ((states[id1]->finalState && !states[id2]->finalState) || (!states[id1]->finalState && states[id2]->finalState))
			grid[distance(statePairs.begin(), i)] = false;
		else
			grid[distance(statePairs.begin(), i)] = true;
	}

	/*
	The grid now marks the False correctly, while the True values at this point simply mark 'blank' spaces, where
	distinguishability has yet to be determined

	The pairs marked as True are the ones we want to investigate, and we can ignore the others
	*/
	int check1, check2;
	recursiveCheck(statePairs, grid);


	/*
	Now any pairs in the grid that are still marked as True will represent indistinguishable states,
	meaning those pairs can be merged into one state

	What needs to happen is that all states that are indistinguishable should be merged, meaning
	if you have a pair of states (2, 6) that are True, and (2, 4) that are also True, then the state will be
	the merger of 2, 4, AND 6.

	So iterate through the remaining True pairs, beginning with the first. Get their IDs, and if they
	share an ID with any other True pairs, put all those IDs into a set
	*/
	
	set<set<int>> theSets;
	set<int> theSet;
	set<int> stateIndicesAffected;
	for (auto k = statePairs.begin(); k != statePairs.end(); k++)
	{
		if (grid[distance(statePairs.begin(), k)])
		{
			id1 = (*k).first;
			id2 = (*k).second;
			if (theSet.empty())
			{
				theSet.insert(id1);
				theSet.insert(id2);
				stateIndicesAffected.insert(id1);
				stateIndicesAffected.insert(id2);
			}
		}
		else
			continue;
		for (auto i = statePairs.begin(); i != statePairs.end(); i++)
		{
			id1 = (*i).first;
			id2 = (*i).second;
			if (grid[distance(statePairs.begin(), i)])
			{
				if (!theSet.contains(id1) && theSet.contains(id2) || theSet.contains(id1) && !theSet.contains(id2))
				{
					theSet.insert(id1);
					theSet.insert(id2);
					stateIndicesAffected.insert(id1);
					stateIndicesAffected.insert(id2);
				}

			}
		}
		if (!theSet.empty()) 
		{
			theSets.insert(theSet);
			theSet.clear();
		}
	}
	
	/*
	With the new states, which are represented as a collection of indices for the states vector,
	those states can be merged together
	*/
	vector<pair<char, int>>* savedTransitions = new vector<pair<char, int>>[theSets.size()];
	vector<State*> tempHolder;

	for (auto k = theSets.begin(); k != theSets.end(); k++)
	{
		int i = distance(theSets.begin(), k);
		State* newS = new State();
		tempHolder.push_back(newS);
		//I only need to check one state in order to determine its transitions
		//this is because, the fact that each of these states is indistinguishable, their individual transitions
		//should line up
		for (auto l = (*k).begin(); l != (*k).end(); l++)
		{
			int index = *l;
			newS->initialState = newS->initialState || states[index]->initialState;
			if (l != (*k).begin())
				continue;
			//if this is a final state
			newS->finalState = newS->finalState || states[index]->finalState;
			


			//need to iterate through the alphabet...
			for (auto m = alphabet.begin(); m != alphabet.end(); m++)
			{
				int nextTransitionID = getNextStateID(*m, states[index]);

				if ((*k).contains(nextTransitionID))
				{
					//this means the new state, newS, should have a self loop on this character
					Transition t(*m, newS);
					newS->transitions.push_back(t);
				}
				else
				{
					//it's possible it needs to transition to an ID that should be merged into separate new state
					//check later. for now, save the transition
					savedTransitions[i].push_back({ *m, nextTransitionID });
				}
			}

		}
	}
	//iterate through the savedTransitions and check the ID of the transitions for if they go to a merged state
	set<int>* skipIndices = new set<int>[theSets.size()];
	for (auto i = theSets.begin(); i != theSets.end(); i++)
	{
		int index = distance(theSets.begin(), i);
		for (auto l = theSets.begin(); l != theSets.end(); l++)
		{
			if (l == i)
				continue;

			for (int k = 0; k < savedTransitions[index].size(); k++)
			{
				int transitionTo = savedTransitions[index][k].second;
				if ((*l).contains(transitionTo))
				{
					int stateIndex = distance(theSets.begin(), l);
					Transition t(savedTransitions[index][k].first, tempHolder[stateIndex]);
					tempHolder[index]->transitions.push_back(t);
					skipIndices[index].insert(k);
					
				}
			}
		}
	}
	
	//Now assign any transitions that remain
	for (auto i = theSets.begin(); i != theSets.end(); i++)
	{
		int index = distance(theSets.begin(), i);
		for (int k = 0; k < savedTransitions[index].size(); k++)
		{
			if (skipIndices[index].contains(k))
				continue;

			int index2 = savedTransitions[index][k].second;
			Transition t(savedTransitions[index][k].first, states[index2]);
			tempHolder[index]->transitions.push_back(t);
			
		}
	}
	delete[] skipIndices;
	/*
	Now that the newly created merged states have their transitions sorted out,
	the other states need to connect to the merged states, and the unnecessary states removed
	*/
	vector<State*> minimized;
	
	for (int i = 0; i < states.size(); i++)
	{
		for (auto k = alphabet.begin(); k != alphabet.end(); k++)
		{
			id1 = getNextStateID(*k, states[i]);
			for (auto l = theSets.begin(); l != theSets.end(); l++)
			{
				if ((*l).contains(i))
					continue;
				//make sure we're not checking a state that's already merged (self-loop)
				if ((*l).contains(id1))
				{
					
					//this state should transition to the merged state on character *k
					//overwrite the old transitions
					for (int n = 0; n < states[i]->transitions.size(); n++)
					{
						
						if (states[i]->transitions[n].acceptedChar == *k)
						{
							states[i]->transitions[n].nextState = tempHolder[distance(theSets.begin(), l)];
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < tempHolder.size(); i++)
		minimized.push_back(tempHolder[i]);
	/*
	With everything connected as needed, remove the states that were merged
	*/
	
	for (auto i = theSets.begin(); i != theSets.end(); i++)
	{
		for (auto l = (*i).begin(); l != (*i).end(); l++)
		{
			delete states[*l];
			states[*l] = nullptr;
		}
	}
	for (int i = 0; i < states.size(); i++)
		if (states[i] != nullptr)
			minimized.push_back(states[i]);

	if (minimized.size() > 0) {
		states.clear();
		int initialStateID = 0;
		for (int i = 0; i < minimized.size(); i++)
		{
			states.push_back(minimized[i]);
			if (states[i]->initialState && i != 0)
				swap(states[0], states[i]);
		}

	}
	minimized.clear();
	delete[] grid;
	
	for (int i = 0; i < theSets.size(); i++)
		savedTransitions[i].clear();
	delete[] savedTransitions;

	for (int i = 0; i < states.size(); i++)
		states[i]->id = i;

	State::stateCounter = states.size() - 1;

	theSets.clear();
	theSet.clear();

}

void Automata::recursiveCheck(set<pair<int, int>>& statePairs, bool* grid)
{
	int id1, id2, check1, check2;
	bool markedSomething = false;
	for (auto i = statePairs.begin(); i != statePairs.end(); i++)
	{
		if (grid[distance(statePairs.begin(), i)])
		{
			id1 = (*i).first;
			id2 = (*i).second;

			for (auto k = alphabet.begin(); k != alphabet.end(); k++)
			{
				check1 = getNextStateID(*k, states[id1]);
				check2 = getNextStateID(*k, states[id2]);
				if (check1 == -1 || check2 == -1)
					continue;
				//check1 and check2 correspond to the state indices
				auto id = statePairs.find({ check1, check2 });
				if (id == statePairs.end())
					id = statePairs.find({ check2, check1 });

				if (id == statePairs.end())
					continue;

				//this pair is marked
				if (!grid[distance(statePairs.begin(), id)])
				{
					//so mark the current pair
					grid[distance(statePairs.begin(), i)] = false;
					//because we found a new thing to mark, we need to run this entire loop chain over again
					markedSomething = true;
				}

			}


		}
	}
	if (markedSomething)
		recursiveCheck(statePairs, grid);
}

int getNextStateID(char theChar, State* current)
{
	for (int i = 0; i < current->transitions.size(); i++)
		if (current->transitions[i].acceptedChar == theChar)
			return current->transitions[i].nextState->id;

	return -1;
}

void characterTransitions(char theChar, State* current, set<State*>& groupedState, int depth, bool useSteps, int steps, set<pair<int, int>> pathTaken)
{
	
	if (current->transitions.size() == 0)
		return;

	if (useSteps)
	{
		if (depth >= steps)
			return;
	}

	for (int i = 0; i < current->transitions.size(); i++)
	{
		if (current->transitions[i].acceptedChar == theChar)
		{		
			if (pathTaken.contains({ current->id, i }))
			return;

		groupedState.insert(current->transitions[i].nextState);
		pathTaken.insert({ current->id, i });
		characterTransitions(theChar, current->transitions[i].nextState, groupedState, depth+1, useSteps, steps, pathTaken);
		}
	}

	
}
