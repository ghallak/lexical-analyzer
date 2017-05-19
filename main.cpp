#include "nfa.h"
#include "dfa.h"

int main()
{
	/*NFA nfa("a(b|c)*");
	DFA dfa(nfa);
	dfa.minimize();*/

	NFA nfa("fee|fie");
	DFA dfa(nfa);
	dfa.print();
	dfa.minimize();
	dfa.print();
}
