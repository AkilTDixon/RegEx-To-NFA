#include <iostream>
#include "Automata.h"
using namespace std;


/*
TODO

NFA creation is rewritten, works in all current cases
converting to DFA is DONE
Minimize DFA is DONE


Brackets within brackets
a(a+(cb)*) --WORKS

a((a+b)c)* --WORKS

a(a+c(dc)*) --WORKS

a(a+(cb)*)* --WORKS

a(a+c(dc)*b) --WORKS

((a+b)(a+b))* --WORKS

a(a+(cb)*c)d --WORKS

(0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*) --WORKS

(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*) --WORKS


((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*)) --WORKS

((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*+(ab+ba)(a+b)*(aa+bb)(a+b)*)* --WORKS

((ab+ba)(a+b)*(aa+bb)(a+b)*)* --WORKS

(a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)* --WORKS

*/

int main()
{
	//string i[20] = {"0","101","1010","1111","10100","11001","11110","100011","101000","101101","110010","110111",
					//"111100","1000001","1000110","1001011","1010000","1010101","1011010","1011111"};

	
	/*
	Figured out the issue:
		When star closure is called, it should create a new intermediary node. Every machine closed under the star should have
		its final states epsilon transition to the new node
		That new node should then epsilon transition to a new "initial state" for the machine, and anything that transitions to
		the old "initial state" should now transition to the new one

		Also, if the old "initial state" was the actual starting state, then mark the original initial state as false, mark the new one
		as true, then swap places in the vector and swap IDs
	*/
	
	/*
	Another issue: the OR

	To fix it, when doing an OR, create a new state. This state will epsilon transition to the rootMachine of the first half,
	and epsilon transition to the next half, becoming the new branching initial state
	*/
	Automata d("((0+1)(0+1)(0+1))*");
	
	//d.input("acb");

	d.convertToDFA();
	//d.input("acb");
	
	
	
	//Automata d('f', true);
	d.minimizeDFA();
	d.print();
	
	return 0;
}