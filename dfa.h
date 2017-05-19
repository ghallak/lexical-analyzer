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

	/**
	 * Create minimal DFA using Hopcroft’s Algorithm.
	 */
	void minimize();

private:
	/**
	 * Update the dfa after applying minimize function.
	 * @param part partition's id for each state
	 * @param parts_count the number of partitions resulting from minimization algorithm
	 */
	void update_dfa(const std::vector<int> & part, int parts_count);

	State *head; /**< The start state for the DFA */
	std::vector<State*> tails; /**< The set of accept states for the DFA */
};
