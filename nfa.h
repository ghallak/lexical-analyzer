#pragma once

#include <vector>
#include <memory>
#include <string>
#include <utility>

#include "finite_automaton.h"
#include "regex.h"

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
	 * @param regex the regular expression
	 */
	explicit NFA(const Regex& regex)
	{ *this = construct(regex.symbols(), 0, regex.symbols().size()); set_states_ids(); }

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
	 * State id for the start state of the NFA.
	 * @return the state id of the start state
	 */
	int start_state() const
	{ return head->state_id(); }

	/**
	 * State id for the accepting state of the NFA.
	 * @return the state id of the accepting state
	 */
	int accepting_state() const
	{ return tail->state_id(); }

private:
	/**
	 * Construct a NFA that corresponds to a single symbol.
	 */
	explicit NFA(const symbol_type & symbol);

	/**
	 * Construct a NFA from a given regular expression.
	 * @param symbols symbols of the given regular expression
	 * @param begin beginning of the regular expression
	 * @param end end of the regular expression + 1
	 * @return A NFA that corresponds to the given regular expression
	 */
	NFA construct(const std::vector<symbol_type> & symbols, int begin, int end);

	/**
	 * The index of the closing paren of an open paren at a given index.
	 * @param symbols the regular expression's symbols to search in
	 * @param idx index to start search after
	 * @return Index of the closing paren
	 */
	int close_index(const std::vector<symbol_type> & symbols, int idx) const;

private:
	State *head; /**< The start state for the NFA */
	State *tail; /**< The accepting state for the NFA */

	mutable std::vector<int> _close_index; /**< Cached values for close_index */
};
