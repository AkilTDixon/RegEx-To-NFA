
#include "pch.h"
#include "Automata.h"
#include "App.h"
#include "MainFrame.h"


class AutomataTest : public testing::Test
{
protected:
	
	void useRegex(string regex) {
		b = new Automata(regex);
	}

	Automata a;
	Automata* b;


};

TEST_F(AutomataTest, CreateAutomata) {
	
  
  EXPECT_EQ(a.DFA, false);
  EXPECT_EQ(a.states.size(), 0);

}

TEST_F(AutomataTest, RegExConstructor) {
	useRegex("a(a+b)");



	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 7);
	delete b;
}

TEST_F(AutomataTest, ORBehavior_createNFA)
{
	useRegex("a+b");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 5);
	EXPECT_EQ(b->states[0]->transitions.size(), 2); // 2 transitions
	//both transitions should be epsilon

	EXPECT_EQ(b->states[0]->transitions[0].acceptedChar, '~'); // epsilon
	EXPECT_EQ(b->states[0]->transitions[1].acceptedChar, '~'); // epsilon

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].acceptedChar, 'a'); // a
	// or
	EXPECT_EQ(b->states[0]->transitions[1].nextState->transitions[0].acceptedChar, 'b'); // b

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->finalState, true);
	EXPECT_EQ(b->states[0]->transitions[1].nextState->transitions[0].nextState->finalState, true);

	delete b;

}

TEST_F(AutomataTest, ANDBehavior_createNFA)
{
	useRegex("ab");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 4);
	EXPECT_EQ(b->states[0]->transitions.size(), 1); // 1 transition
	
	//transition should be on a
	EXPECT_EQ(b->states[0]->transitions[0].acceptedChar, 'a'); // a

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions.size(), 1); // 1 transition
	
	//transition should be on epsilon
	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].acceptedChar, '~'); // ~


	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions.size(), 1); // 1 transition
	
	//transition should be on b
	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].acceptedChar, 'b'); // b

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].nextState->finalState, true); // should be final state


	delete b;

}

TEST_F(AutomataTest, StarClosureBehavior_createNFA)
{
	useRegex("a*");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 4);
	EXPECT_EQ(b->states[0]->transitions.size(), 1); // 1 transition

	//transition should be on ~
	EXPECT_EQ(b->states[0]->transitions[0].acceptedChar, '~'); // ~

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions.size(), 1); // 1 transition

	//transition should be on a
	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].acceptedChar, 'a'); // a


	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions.size(), 1); // 1 transition

	//transition should be on ~
	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].acceptedChar, '~'); // ~

	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].nextState->transitions.size(), 1);
	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].nextState->transitions[0].acceptedChar, '~');


	EXPECT_EQ(b->states[0]->transitions[0].nextState->transitions[0].nextState->transitions[0].nextState->transitions[0].nextState->finalState, true); // should be final state


	delete b;

}

TEST_F(AutomataTest, ComplexRegEx1_createNFA)
{
	useRegex("(0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 49);

	b->convertToDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 44);

	b->minimizeDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 4);

	delete b;

}

TEST_F(AutomataTest, ComplexRegEx2_createNFA)
{
	useRegex("(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*)");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 41);

	b->convertToDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 39);

	b->minimizeDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 11);

	delete b;

}

TEST_F(AutomataTest, ComplexRegEx3_createNFA)
{
	useRegex("((ab+ba)(a+b)*(aa+bb)(a+b)*)*");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 34);

	b->convertToDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 16);

	b->minimizeDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 8);

	delete b;

}
TEST_F(AutomataTest, ComplexRegEx4_createNFA)
{
	useRegex("(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 39);

	b->convertToDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 16);

	b->minimizeDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 8);

	delete b;

}

TEST_F(AutomataTest, ComplexRegEx5_createNFA)
{
	useRegex("((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*+(ab+ba)(a+b)*(aa+bb)(a+b)*)*");

	EXPECT_EQ(b->DFA, false);
	EXPECT_EQ(b->states.size(), 74);

	b->convertToDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 52);

	b->minimizeDFA();
	EXPECT_EQ(b->DFA, true);
	EXPECT_EQ(b->states.size(), 15);

	delete b;

}
TEST_F(AutomataTest, TestReset_reset)
{
	useRegex("((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*+(ab+ba)(a+b)*(aa+bb)(a+b)*)*");

	b->reset();
	EXPECT_EQ(b->states.size(), 0);
	EXPECT_EQ(b->DFA, false);

	EXPECT_EQ(b->miniMachines.size(), 0);
	EXPECT_EQ(b->alphabet.size(), 0);

	delete b;

}

//TEST_F(AutomataTest, ComplexRegExFinal) {
//	useRegex("((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*))");
//
//
//
//	EXPECT_EQ(b->DFA, false);
//	EXPECT_EQ(b->states.size(), 91);
//
//	b->convertToDFA();
//	EXPECT_EQ(b->DFA, true);
//	EXPECT_EQ(b->states.size(), 81);
//
//	b->minimizeDFA();
//	EXPECT_EQ(b->DFA, true);
//	EXPECT_EQ(b->states.size(), 12);
//
//	delete b;
//}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}