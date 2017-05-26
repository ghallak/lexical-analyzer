#include "regex.h"

Regex::Regex(const std::string& regex)
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

Regex::Symbol::Symbol(const std::string& symbol)
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
			// TODO: change exception
			throw std::exception();
	}
}

std::string Regex::Symbol::to_string() const
{
	switch (_type)
	{
		case Type::EPS:         return "EPSILON";
		case Type::OPEN_PAREN:  return "(";
		case Type::CLOSE_PAREN: return ")";
		case Type::KLEEN_STAR:  return "*";
		case Type::UNION_OP:    return "|";
		default:                return _symbol;
	}
}
