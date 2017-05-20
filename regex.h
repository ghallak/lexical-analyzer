#pragma once

#include <string>
#include <vector>

class Regex
{
public:
	class Symbol
	{
	private:
		enum class Type
		{
			EPS,
			CHAR,
			RANGE,
			OPEN_PAREN,
			CLOSE_PAREN,
			KLEEN_STAR,
			UNION_OP
		};

	public:
		Symbol(const std::string & symbol = "");
		Symbol(char c);

		// TODO: Should I overload all operators ??
		// try to use undefined operator
		bool operator==(const Symbol & rhs) const;

		bool is_epsilon() const;
		bool is_char() const;
		bool is_range() const;
		bool is_kleen_star() const;
		bool is_union_op() const;
		bool is_open_paren() const;
		bool is_close_paren() const;

		std::string to_string() const;

	private:
		std::string _symbol;
		Type _type;
	};

public:
	explicit Regex(const std::string & regex);

	const std::vector<Symbol> & symbols() const;

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

		result_type operator()(const argument_type & arg) const
		{
			return hash<string>()(arg.to_string());
		}
	};
}
