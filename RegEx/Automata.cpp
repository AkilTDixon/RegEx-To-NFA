#include "Automata.h";
#include <iostream>
#include <set>
#include <unordered_set>
#include <utility>
#include <stack>
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
	initialState = false;
	
}
State::State(bool fnState)
{
	stateCounter++;
	id = stateCounter;
	finalState = fnState;
	loopStart = false;
	initialState = false;
}


Automata::Automata()
{

	DFA = false;
	//alphabet.insert('~');
	

}

Automata::Automata(string regEx)
{
	int depth = 0;


	DFA = false;
	//alphabet.insert('~');
	

	createNFA(regEx);

	
}
Automata::Automata(char fff, bool testingPurposes)
{
	State* initial = new State();
	states.push_back(initial);
	initial->initialState = true;
	DFA = true;
	//alphabet.insert('~');

	State* n2 = new State();
	states.push_back(n2);
	State* n3 = new State();
	states.push_back(n3);
	n3->finalState = true;
	State* n4 = new State();
	states.push_back(n4);
	State* n5 = new State();
	states.push_back(n5);
	n5->finalState = true;
	State* n6 = new State();
	states.push_back(n6);

	alphabet.insert('a');
	alphabet.insert('b');

	Transition t1('a',n2);
	initial->transitions.push_back(t1);
	Transition t2('b', n4);
	initial->transitions.push_back(t2);

	Transition t3('a', n4);
	n2->transitions.push_back(t3);
	Transition t4('b', n3);
	n2->transitions.push_back(t4);

	Transition t5('a', n3);
	n3->transitions.push_back(t5);
	Transition t6('b', n3);
	n3->transitions.push_back(t6);

	Transition t7('a', n4);
	n4->transitions.push_back(t7);
	Transition t8('b', n5);
	n4->transitions.push_back(t8);

	Transition t9('a', n5);
	n5->transitions.push_back(t9);
	Transition t10('b', n5);
	n5->transitions.push_back(t10);

	Transition t11('a', n6);
	n6->transitions.push_back(t11);
	Transition t12('b', n5);
	n6->transitions.push_back(t12);

	//trap state dummy
	//State* newState = new State();
	//states.push_back(newState);
	//Transition t13('a', newState);
	//Transition t14('b', newState);
	//newState->transitions.push_back(t13);
	//newState->transitions.push_back(t14);
}


Automata::~Automata()
{
	
}

void Automata::reset()
{
	for (auto s : states)
		delete s;

	states.clear();

	State::stateCounter = -1;

}

void Automata::reset(string regEx)
{
	for (auto s : states)
		delete s;

	states.clear();

	State::stateCounter = -1;

	State* initial = new State();
	states.push_back(initial);


	int depth = 0;
	createNFA(regEx);

}

void Automata::input(string s)
{
	int depth = 0;
	bool accepted = testInput(s, s[depth], depth, s.length(), states[0]);
	if (accepted)
		cout << "Accepted" << endl;
	else
		cout << "Rejected" << endl;
}

//simple. needs work for complex Automata transitions
void Automata::print()
{
	
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
	{
		return current->finalState;
	}
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
	vector<State*> mini;

	//trying to follow a more algorithmic approach
	for (int i = 0; i < regex.length(); i++)
	{
		
		currCharacter = regex[i];
		//making mini NFAs for each literal
		if (currCharacter != '(' && currCharacter != '*' && currCharacter != '+' && currCharacter != ')')
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
				{
					newS->initialState = true;
				}
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
				
				
				if (rootMachine != -1)
				{
					for (int k = rootMachine; k < miniMachines.size(); k++)
					{
						for (int l = 0; l < miniMachines[k].size(); l++)
						{
							if (miniMachines[k][l]->finalState)
							{
								Transition e('~', miniMachines[rootMachine][0]);
								miniMachines[k][l]->transitions.push_back(e);
								miniMachines[k][l]->finalState = false;
							}
						}
					}
					miniMachines[rootMachine][0]->finalState = true;
					
				}
				else
				{

					//then the previously saved minimachine must be looped
					//by putting an epsilon transition to its initial state from its final state
					int miniMachineIndex = miniMachines.size() - 1;
					int innerIndex = miniMachines[miniMachines.size() - 1].size() - 1;
					Transition t('~', miniMachines[miniMachineIndex][0]);

					miniMachines[miniMachineIndex][innerIndex]->transitions.push_back(t);
					if (miniMachines[miniMachineIndex][innerIndex]->finalState)
					{
						miniMachines[miniMachineIndex][innerIndex]->finalState = false;
						miniMachines[miniMachineIndex][0]->finalState = true;
					}
				}
			}
			else if (currCharacter == '+')
			{
				//then the previously saved minimachine's initial state must branch into this next machine
				branching = true;
				bracketEnd = false;
				
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
				branchingBracket = true;
				//not in a bracket
				if (!bracketCount.empty())
					rootMachine = bracketCount.top();
				else
					rootMachine = 0;

			}
			

			bracketCount.push(miniMachines.size());
			//the first machine inside the bracket will be the next created one
			
		}
		if (!mini.empty())
			miniMachines.push_back(mini);

		mini.clear();

	}

	int upperLimit;
	//put the machines together using epsilon transitions
	//for (int i = 0; i < miniMachines.size()-1; i++)
	//{
	//	upperLimit = miniMachines[i].size() - 1;

	//	Transition e('~', miniMachines[i + 1][0]);
	//	miniMachines[i][upperLimit]->transitions.push_back(e);
	//	

	//	
	//}
	for (int i = 0; i < miniMachines.size(); i++)
	{
		for (int j = 0; j < miniMachines[i].size(); j++)
			states.push_back(miniMachines[i][j]);
		
		miniMachines[i].clear();
	}

	miniMachines.clear();


}




void Automata::convertToDFA()
{
	vector<set<State*>> p;
	set<set<State*>> masterList;
	set<State*> groupedState;
	set<pair<int, int>> pathTaken;

	groupedState.insert(states[0]);

	/*
	Starting with the initial state, create a new grouped state that's the result of any 
	and all epsilon transitions from q0

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
			{

				characterTransitions(*k, *i, groupedState, 0, true, 1, pathTaken);

			}
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
			{
				
				characterTransitions(*k, *l, groupedState, 0, true, 1, pathTaken);

			}
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
				{
					
					allTransitions.push_back({{i, distance(p.begin(), it)}, *k});
				}
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
			if ((*k)->initialState)
				initialStateID = i;
			if ((*k)->finalState) 
				fnStates[i] = true;
			

		}
	}
	masterList.clear();
	p.clear();
	reset();

	//initial state already exists, the extra state at the end will be used for the trap state
	//states[numOfStates] = trap state
	
	for (int i = 0; i < numOfStates+1; i++)
	{
		State* newState = new State();
		newState->finalState = fnStates[i];	
		if (i == initialStateID)
			newState->initialState = true;
		states.push_back(newState);
		if (newState->initialState && i != 0)
			swap(states[0], states[i]);

		
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
		//skip assigning to trap state
		//if (to == numOfStates)
			//continue;
		theChar = allTransitions[i].second;

		
		Transition t(theChar, states[to]);
		states[from]->transitions.push_back(t);
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
	//GOOD UP TO HERE
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
	//GOOD UP TO HERE - THE TRANSITIONS TO SET ARE PROPER
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
	//GOOD UP TO HERE. IF THE MERGED SETS HAVE TRANSITIONS TO EACH OTHER, THEY ARE SET PROPERLY
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
					//because we found a new thing to mark, we need to run this entire for loop chain over again
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
/*


a(ab+c)

q0 -a-> q1

what if I made the inside of the brackets a sub Automata, who's initial state is connected to by the current state

(
put everything in the brackets into a new string
)

Do a new recursive call using the new string, with the current node which is q1
ab+c

q1 -a-> q2 -b-> |q3|
q1 -c-> |q4|


q0 -a-> q1 -a-> q2 -b-> |q3|
		q1 -c-> |q4|


The OR should work by calling the recursive function for each character/string that follows +



*/