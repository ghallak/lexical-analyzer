#include "regex.h"
#include "regex_tree.h"
#include "dfa.h"

#include <string>

#include <iostream>
using std::cout;
using std::endl;

int main()
{
	AugmentedRegexTree x(AugmentedRegex("(ab)#|(c)#"));
	DFA dfa(x);
	dfa.print();
}
