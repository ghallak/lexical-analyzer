#pragma once

#include <vector>

#include "finite_automaton.h"
#include "regex_tree.h"

/**
 * A class for deterministic finite automaton.
 */
class DFA : public FiniteAutomaton
{
public:
	/**
	 * Convert an augmented regular expression tree directly to DFA
	 * @param tree augmented regular expression tree
	 */
	explicit DFA(const AugmentedRegexTree& tree);

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
};
