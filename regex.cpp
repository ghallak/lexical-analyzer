#include "regex.h"

#include "utility.h"

Symbol::Symbol(const std::string& symbol)
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

std::string Symbol::to_string() const
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

RegexTree::Node::Node(RegexTree::NodeType type,
                      std::unique_ptr<RegexTree::Node> left,
                      std::unique_ptr<RegexTree::Node> right)
	: _type(type)
{
	switch (type)
	{
		case NodeType::CONCAT:
		case NodeType::UNION:
			_children.emplace_back(std::move(left));
			_children.emplace_back(std::move(right));
			break;
		case NodeType::STAR:
			_children.emplace_back(std::move(left));
			break;
		default:
			// TODO: throw another exception: can't make node of this type
			throw std::exception();
	}
}

RegexTree::Node::Node(RegexTree::NodeType type, Symbol label, leaf_pos_type pos)
	: _type(type), _label(label), _leaf_pos(pos)
{
	if (type != NodeType::LEAF)
		// TODO: throw exception: can't make node of this type
		throw std::exception();
}

RegexTree::RegexTree(const std::string& regex)
{
	std::vector<Symbol> symbols;
	for (std::size_t i = 0; i < regex.length();)
	{
		if (i + 2 < regex.length() && regex[i + 1] == '-')
		{
			symbols.emplace_back(regex.substr(i, 3));
			i += 3;
		}
		else
		{
			symbols.emplace_back(regex[i]);
			++i;
		}
	}

	// Calculate the closing parens indices for each open paren
	calc_close_index(symbols);

	// Set the root for the RegexTree
	root = init(symbols, 0, symbols.size());

	// Calculate nullable
	calc_nullable(root.get());

	// Calculate firstpos and lastpos
	calc_first_last_pos(root.get());

	// Calculate followpos for the RegexTree
	calc_followpos(root.get());
}

std::vector<Symbol> RegexTree::alphabet() const
{
	std::unordered_set<Symbol> labels;
	for (auto leaf : _leaves)
		labels.insert(leaf->label());
	return std::vector<Symbol>(labels.begin(), labels.end());
}

std::unique_ptr<RegexTree::Node> RegexTree::init(
	const std::vector<Symbol>& symbols,
	std::size_t begin,
	std::size_t end)
{
	if (begin >= end)
	{
		// TODO: report error: invalid regular expression
		throw std::exception();
	}

	// .
	if (begin + 1 == end)
	{
		auto leaf =  std::make_unique<Node>(NodeType::LEAF,
		                                    symbols[begin],
		                                    _leaves.size());
		_leaves.emplace_back(leaf.get());
		return std::move(leaf);
	}

	if (symbols[begin].is_open_paren())
	{
		auto close = close_index[begin];

		// (...)*...
		if (close + 1 < end && symbols[close + 1].is_kleen_star())
		{
			// (...)*...
			if (close + 2 < end)
			{
				return std::make_unique<Node>(NodeType::CONCAT,
				                              init(symbols, begin, close + 2),
				                              init(symbols, close + 2, end));
			}
			// (...)*
			else
			{
				return std::make_unique<Node>(NodeType::STAR,
				                              init(symbols, begin + 1, close));
			}
		}
		// (...)...
		else
		{
			// (...)...
			if (close + 1 < end)
			{
				return std::make_unique<Node>(NodeType::CONCAT,
				                              init(symbols, begin + 1, close),
				                              init(symbols, close + 1, end));
			}
			// (...)
			else
			{
				return init(symbols, begin + 1, close);
			}
		}
	}

	// ...|...
	int depth = 0;
	for (std::size_t i = begin; i < end; ++i)
	{
		depth += symbols[i].is_open_paren();
		depth -= symbols[i].is_close_paren();
		if (symbols[i].is_union_op() && depth == 0)
		{
			return std::make_unique<Node>(NodeType::UNION,
			                              init(symbols, begin, i),
			                              init(symbols, i + 1, end));
		}
	}

	// .*...
	if (begin + 1 < end && symbols[begin + 1].is_kleen_star())
	{
		// .*...
		if (begin + 2 < end)
		{
			return std::make_unique<Node>(NodeType::CONCAT,
			                              init(symbols, begin, begin + 2),
			                              init(symbols, begin + 2, end));
		}
		// .*
		else
		{
			return std::make_unique<Node>(NodeType::STAR,
			                              init(symbols, begin, begin + 1));
		}
	}

	// ...
	return std::make_unique<Node>(NodeType::CONCAT,
	                              init(symbols, begin, begin + 1),
	                              init(symbols, begin + 1, end));
}

RegexTree::Node* RegexTree::Node::left() const
{
	if (_type == NodeType::CONCAT || _type == NodeType::UNION)
	{
		return _children[0].get();
	}
	// TODO: throw exception: node has no left child
	throw std::exception();
}

RegexTree::Node* RegexTree::Node::right() const
{
	if (_type == NodeType::CONCAT || _type == NodeType::UNION)
	{
		return _children[1].get();
	}
	// TODO: throw exception: node has no right child
	throw std::exception();
}

RegexTree::Node* RegexTree::Node::child() const
{
	if (_type == NodeType::STAR)
	{
		return _children[0].get();
	}
	// TODO: throw exception: node has no children
	throw std::exception();
}

RegexTree::leaf_pos_type RegexTree::Node::leaf_pos() const
{
	if (_type == NodeType::LEAF)
	{
		return _leaf_pos;
	}
	// TODO: throw exception: node is not a leaf
	throw std::exception();
}

Symbol RegexTree::Node::label() const
{
	if (_type == NodeType::LEAF)
	{
		return _label;
	}
	// TODO: throw exception: node is not a leaf
	throw std::exception();
}

void RegexTree::calc_close_index(const std::vector<Symbol>& symbols)
{
	close_index.resize(symbols.size(), symbols.size());

	std::vector<int> stack;
	for (std::size_t i = 0; i < symbols.size(); ++i)
	{
		if (symbols[i].is_open_paren())
		{
			stack.emplace_back(i);
		}
		else if (symbols[i].is_close_paren())
		{
			close_index[stack.back()] = i;
			stack.pop_back();
		}
	}
}

void RegexTree::calc_nullable(RegexTree::Node* node)
{
	if (node->is_leaf())
		node->nullable = false;
	else if (node->is_star())
		node->nullable = true;
	else if (node->is_union() || node->is_concat())
	{
		calc_nullable(node->left());
		calc_nullable(node->right());
		if (node->is_union())
			node->nullable = node->left()->nullable | node->right()->nullable;
		else
			node->nullable = node->left()->nullable & node->right()->nullable;
	}
	else
		// TODO: throw exception: node is of undefined type
		throw std::exception();
}

void RegexTree::calc_first_last_pos(RegexTree::Node* node)
{
	if (node->is_leaf())
	{
		node->firstpos.insert(node->leaf_pos());
		node->lastpos.insert(node->leaf_pos());
	}
	else if (node->is_union() || node->is_concat())
	{
		calc_first_last_pos(node->left());
		calc_first_last_pos(node->right());

		if (node->is_union() || node->left()->nullable)
		{
			node->firstpos = utility::union_sets(node->left()->firstpos,
			                                     node->right()->firstpos);
		}
		else
		{
			node->firstpos = node->left()->firstpos;
		}
		if (node->is_union() || node->right()->nullable)
		{
			node->lastpos = utility::union_sets(node->left()->lastpos,
			                                    node->right()->lastpos);
		}
		else
		{
			node->lastpos = node->right()->lastpos;
		}
	}
	else if (node->is_star())
	{
		calc_first_last_pos(node->child());
		node->firstpos = node->child()->firstpos;
		node->lastpos = node->child()->lastpos;
	}
	else
		// TODO: throw exception: node is of undefined type
		throw std::exception();
}

void RegexTree::calc_followpos(RegexTree::Node* node)
{
	if (node->is_union())
	{
		calc_followpos(node->left());
		calc_followpos(node->right());
	}
	if (node->is_concat())
	{
		for (auto fpos : node->right()->firstpos)
		{
			for (auto lpos : node->left()->lastpos)
			{
				_leaves[lpos]->followpos.insert(fpos);
			}
		}
		calc_followpos(node->left());
		calc_followpos(node->right());
	}
	if (node->is_star())
	{
		for (auto lpos : node->child()->lastpos)
		{
			_leaves[lpos]->followpos.insert(node->child()->firstpos.cbegin(),
			                                node->child()->firstpos.cend());
		}
		calc_followpos(node->child());
	}
}
