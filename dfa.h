#pragma once

#include <vector>

#include "finite_automaton.h"
#include "nfa.h"

/**
 * A class for deterministic finite automaton.
 */
class DFA : public FiniteAutomaton
{
public:
	/**
	 * Subset construction algorithm.
	 * @param nfa NFA to construct a DFA from
	 */
	explicit DFA(const NFA& nfa);

private:
	State *head; /**< The start state for the DFA */
	std::vector<State*> tails; /**< The set of accept states for the DFA */
};
