#include "nfa.h"

int main()
{
	auto nfa = NFA("a(b|c)*");
	nfa.print();
	nfa.eps_closure(3);
}
