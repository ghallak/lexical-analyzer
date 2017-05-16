#pragma once

#include <vector>
#include <memory>
#include <string>
#include <utility>

#include "finite_automaton.h"

/**
 * A class for nondeterministic finite automaton.
 */
class NFA : public FiniteAutomaton
{
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
	 * @return boolean vector with true value for each state that is contained the epsilon closure
	 */
	eps_closure_type eps_closure(int state) const;

	/**
	 * State number for the start state of the NFA.
	 * @return the state number of the start state
	 */
	int start_state() const
	{ return head->state_number; }

	/**
	 * State number for the accepting state of the NFA.
	 * @return the state number of the accepting state
	 */
	int accepting_state() const
	{ return tail->state_number; }

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
	 * The index of the closing paren of an open paren at a given index.
	 * @param s the string to search in
	 * @param idx index to start search after
	 * @return Index of the closing paren
	 */
	int close_index(const std::string& s, int idx) const;

private:
	State *head; /**< The start state for the NFA */
	State *tail; /**< The accepting state for the NFA */

	mutable std::vector<int> _close_index; /**< Cached values for close_index */
};
