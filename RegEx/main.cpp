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


((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*)) -- WORKS





*/

int main()
{
	string i[20] = {"0","101","1010","1111","10100","11001","11110","100011","101000","101101","110010","110111",
					"111100","1000001","1000110","1001011","1010000","1010101","1011010","1011111"};

	

	Automata d("((0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)+(1(0(0(1(0+1)(0+1)*)*0+1(0+1)(0+1)*))*))");

	//d.input("acb");

	d.convertToDFA();
	//d.input("acb");

	
	
	//Automata d('f', true);
	d.minimizeDFA();
	for (int c = 0; c < 20; c++)
		d.input(i[c]);
	return 0;
}