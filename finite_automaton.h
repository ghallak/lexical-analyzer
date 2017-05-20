#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "regex.h"

/////////DELETE LATER//////////////
#include <iostream>
using std::cout;
using std::endl;
///////////////////////////////////

/**
 * A class for finite automaton.
 */
class FiniteAutomaton
{
public:
	using symbol_type = Regex::Symbol;
	using eps_closure_type = std::vector<bool>;

protected:
	/**
	 * Give unique id to each state of the FiniteAutomaton states.
	 */
	void set_states_ids();

	/**
	 * A class that represents a state of a FiniteAutomaton.
	 */
	class State
	{
	private:
		struct Transition
		{
			Transition(const State* state, const symbol_type & symbol)
				: _state(state), _symbol(symbol)
			{ }

			const State* state() const
			{ return _state; }

			symbol_type symbol() const
			{ return _symbol; }

		private:
			const State* _state;
			symbol_type _symbol;
		};

	public:
		/**
		 * Returns the id of the state.
		 */
		int state_id() const
		{ return _state_id; }

		/**
		 * Returns a const reference to transitions vector.
		 */
		const std::vector<Transition> & transitions() const
		{ return _transitions; }

		/**
		 * Adds a transition from the state.
		 * @param state the state to add a transition to
		 * @param symbol the symbol lable of the transition
		 */
		void add_transition(State* state, const symbol_type & symbol = symbol_type())
		{ _transitions.push_back(Transition(state, symbol)); }

		/**
		 * Updates the transition at a given index.
		 * @param idx the given index
		 * @param state the new state
		 * @param symbol the new symbol
		 */
		void update_transition(std::size_t idx, State* state, const symbol_type & symbol)
		{ _transitions[idx] = Transition(state, symbol); }

		friend void FiniteAutomaton::set_states_ids();

	private:
		int _state_id; /**< unique id for each states */
		std::vector<Transition> _transitions; /**< all transitions from a state */
	};

public:
	/**
	 * Check if the FiniteAutomaton is empty.
	 * @return true when the FiniteAutomaton is empty, and false otherwise.
	 */
	bool empty() const
	{ return states.empty(); }

	/**
	 * The number of states in the automaton.
	 * @return the number of states
	 */
	int states_count() const
	{ return states.size(); }

	/**
	 * Getter for the alphabet of the FiniteAutomaton.
	 * @return the alphabet of the FiniteAutomaton
	 */
	const std::vector<symbol_type> & alphabet() const;

	/**
	 * Transition from a state to another through a symbol
	 * @param state_id the id of the state to look from
	 * @param symbol the symbol of the transition
	 * @return the id of the founded state and -1 if no state is found
	 */
	int transition(int state_id, const symbol_type & symbol) const;

	/**
	 * Print the FiniteAutomaton.
	 */
	void print() const;

protected:
	std::vector<std::unique_ptr<State>> states; /**< The states of the FiniteAutomaton */
	mutable std::vector<symbol_type> _alphabet; /**< Cached alphabet */
};
