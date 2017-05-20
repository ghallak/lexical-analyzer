#include "regex.h"

#include <string>

Regex::Symbol::Symbol(const std::string & symbol)
{
	switch (symbol.length())
	{
		case 0:
			_type = Type::EPS;
			break;

		case 1:
			switch (symbol[0])
			{
				case '|':
					_type = Type::UNION_OP;
					break;
				case '*':
					_type = Type::KLEEN_STAR;
					break;
				case '(':
					_type = Type::OPEN_PAREN;
					break;
				case ')':
					_type = Type::CLOSE_PAREN;
					break;
				default:
					_symbol = symbol;
					_type = Type::CHAR;
			}
			break;

		case 3:
			if (symbol[1] != '-') throw std::exception();

			_symbol = symbol;
			_type = Type::RANGE;
			break;

		default:
			throw std::exception();
	}
}

Regex::Symbol::Symbol(char c) : Symbol(std::string(1, c))
{
}

bool Regex::Symbol::operator==(const Symbol & rhs) const
{
	return _symbol == rhs._symbol && _type == rhs._type;
}

bool Regex::Symbol::is_epsilon() const
{
	return _type == Regex::Symbol::Type::EPS;
}

bool Regex::Symbol::is_char() const
{
	return _type == Regex::Symbol::Type::CHAR;
}

bool Regex::Symbol::is_range() const
{
	return _type == Regex::Symbol::Type::RANGE;
}

bool Regex::Symbol::is_kleen_star() const
{
	return _type == Regex::Symbol::Type::KLEEN_STAR;
}

bool Regex::Symbol::is_union_op() const
{
	return _type == Regex::Symbol::Type::UNION_OP;
}

bool Regex::Symbol::is_open_paren() const
{
	return _type == Regex::Symbol::Type::OPEN_PAREN;
}

bool Regex::Symbol::is_close_paren() const
{
	return _type == Regex::Symbol::Type::CLOSE_PAREN;
}

std::string Regex::Symbol::to_string() const
{
	return _symbol;
}

Regex::Regex(const std::string & regex)
{
	for (std::size_t i = 0; i < regex.length();)
	{
		if (i + 2 < regex.length() && regex[i + 1] == '-')
		{
			_symbols.emplace_back(regex.substr(i, 3));
			i += 3;
		}
		else
		{
			_symbols.emplace_back(regex[i]);
			++i;
		}
	}
}

const std::vector<Regex::Symbol> & Regex::symbols() const
{
	return _symbols;
}
