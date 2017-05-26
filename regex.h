#pragma once

#include <vector>
#include <string>

class Regex
{
public:
	class Symbol
	{
	public:
		Symbol(const std::string& symbol = "");

		Symbol(char c) : Symbol(std::string(1, c))
		{ }


		bool operator==(const Symbol& rhs) const
		{ return _symbol == rhs._symbol && _type == rhs._type; }

		bool operator!=(const Symbol& rhs) const
		{ return !(*this == rhs); }


		bool is_epsilon() const
		{ return _type == Symbol::Type::EPS; }

		bool is_char() const
		{ return _type == Symbol::Type::CHAR; }

		bool is_range() const
		{ return _type == Symbol::Type::RANGE; }

		bool is_kleen_star() const
		{ return _type == Symbol::Type::KLEEN_STAR; }

		bool is_union_op() const
		{ return _type == Symbol::Type::UNION_OP; }

		bool is_open_paren() const
		{ return _type == Symbol::Type::OPEN_PAREN; }

		bool is_close_paren() const
		{ return _type == Symbol::Type::CLOSE_PAREN; }

		std::string to_string() const;

	private:
		enum class Type
		{
			EPS, CHAR, RANGE, OPEN_PAREN, CLOSE_PAREN, KLEEN_STAR, UNION_OP
		};


		std::string _symbol;
		Type _type;
	};


	explicit Regex(const std::string& regex);


	std::vector<Symbol> symbols() const
	{ return _symbols; }

private:
	std::vector<Symbol> _symbols;
};

namespace std
{
	template<>
	struct hash<::Regex::Symbol>
	{
		using argument_type = ::Regex::Symbol;
		using result_type = size_t;

		result_type operator()(const argument_type& arg) const
		{ return hash<string>()(arg.to_string()); }
	};
}

class AugmentedRegex : public Regex
{
public:
	explicit AugmentedRegex(std::string regex)
	: Regex('(' + regex + ')' + '#')
	{ }
};
