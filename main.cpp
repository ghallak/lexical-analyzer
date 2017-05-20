#include "nfa.h"
#include "dfa.h"
#include "regex.h"

int main()
{
	//NFA nfa("a(b|c)*");
	//DFA dfa(nfa);
	//dfa.minimize();

	//std::string regex ="fee|fie";
	Regex regex("a-zba");
	NFA nfa(regex);
	DFA dfa(nfa);
	nfa.print();
	dfa.minimize();
	dfa.print();
}
