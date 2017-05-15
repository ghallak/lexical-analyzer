#pragma once

#include <vector>
#include <memory>
#include <string>
#include <utility>

/**
 * A class for nondeterministic finite automata.
 */
class NFA
{
private:
	/**
	 * A class representing a state of a nondeterministic finite automata.
	 */
	struct State
	{
		int state_number; /**< unique number for each states */
		std::vector<std::pair<char, State*>> adjacent; /**< all adjacent states */
	};

public:
	/**
	 * Construct an empty NFA.
	 */
	NFA() : head(nullptr), tail(nullptr)
	{ }

	/**
	 * Construct a NFA from a given regular expression.
	 * @param s the regular expression
	 */
	explicit NFA(const std::string& s);

	/**
	 * Construct a NFA that corresponds to a single character.
	 */
	explicit NFA(char c);

	/**
	 * Check if the NFA is empty.
	 * @return true if the NFA is empty, and false otherwise.
	 */
	bool empty()
	{ return head == tail && head == nullptr; }

	/**
	 * Concatenate two NFAs.
	 * @param rhs the NFA to be concatenated with the current one.
	 */
	NFA& operator+=(NFA&& rhs);

	/**
	 * Union two NFAs.
	 * @param rhs the NFA to be unioned with the current one.
	 */
	NFA& operator|=(NFA&& rhs);

	/**
	 * The Kleen closure of the NFA.
	 */
	NFA&& operator*() &&;

	/**
	 * Calculate the epsilon closure for a given state.
	 * @param state the state to calculate its epsilon closure
	 */
	void eps_closure(int state);

	/**
	 * Print the NFA.
	 */
	void print();

private:
	/**
	 * Construct a NFA from a given regular expression.
	 * @param s the given regular expression
	 * @param begin beginning of the regular expression
	 * @param end end of the regular expression + 1
	 * @return A NFA that corresponds to the given regular expression
	 */
	NFA construct(const std::string& s, int begin, int end);

	/**
	 * Find the first closing paren after a given index.
	 * @param s the string to search in
	 * @param idx index to start search after
	 * @return Index of the first closing paren
	 */
	int find_close(const std::string& s, int idx);

private:
	State *head; /**< The start state for the NFA */
	State *tail; /**< The accepting state for the NFA */
	std::vector<std::unique_ptr<State>> states; /**< The states of the NFA */
};
