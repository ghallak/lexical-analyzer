#include "nfa.h"
#include "dfa.h"

int main()
{
	NFA nfa("a(b|c)*");
	DFA dfa(nfa);
}
