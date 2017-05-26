#include "regex_tree.h"

#include "utility.h"

RegexTree::RegexTree(const Regex& regex)
{
	// Calculate the closing parens indices for each open paren
	calc_close_index(regex.symbols());

	// Set the root for the RegexTree
	root = init<Node>(regex.symbols(), 0, regex.symbols().size());
}

RegexTree::Node::Node(Type type,
                      std::unique_ptr<Node> left,
                      std::unique_ptr<Node> right)
	: _type(type)
{
	switch (type)
	{
		case Type::CONCAT:
		case Type::UNION:
			_children.emplace_back(std::move(left));
			_children.emplace_back(std::move(right));
			break;
		case Type::STAR:
			_children.emplace_back(std::move(left));
			break;
		default:
			// TODO: throw another exception: can't make node of this type
			throw std::exception();
	}
}

RegexTree::Node::Node(Type type, symbol_type label)
	: _type(type), _label(label)
{
	if (type != Type::LEAF)
		// TODO: throw exception: can't make node of this type
		throw std::exception();
}

RegexTree::Node* RegexTree::Node::left() const
{
	if (_type == Type::CONCAT || _type == Type::UNION)
	{
		return _children[0].get();
	}
	// TODO: throw exception: node has no left child
	throw std::exception();
}

RegexTree::Node* RegexTree::Node::right() const
{
	if (_type == Type::CONCAT || _type == Type::UNION)
	{
		return _children[1].get();
	}
	// TODO: throw exception: node has no right child
	throw std::exception();
}

RegexTree::Node* RegexTree::Node::child() const
{
	if (_type == Type::STAR)
	{
		return _children[0].get();
	}
	// TODO: throw exception: node has no children
	throw std::exception();
}

RegexTree::symbol_type RegexTree::Node::label() const
{
	if (_type == Type::LEAF)
	{
		return _label;
	}
	// TODO: throw exception: node is not a leaf
	throw std::exception();
}

void RegexTree::calc_close_index(const std::vector<symbol_type>& symbols)
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

AugmentedRegexTree::leaf_pos_type AugmentedRegexTree::Node::leaf_pos() const
{
	if (_type == Node::Type::LEAF)
	{
		return _leaf_pos;
	}
	// TODO: throw exception: node is not a leaf
	throw std::exception();
}

AugmentedRegexTree::AugmentedRegexTree(const AugmentedRegex& regex)
{
	// Calculate the closing parens indices for each open paren
	calc_close_index(regex.symbols());

	// Set the root for the RegexTree
	root = init<Node>(regex.symbols(), 0, regex.symbols().size());

	for (std::size_t i = 0; i < _leaves.size(); ++i)
	{
		static_cast<Node*>(_leaves[i])->_leaf_pos = i;
	}

	auto root_node = static_cast<Node*>(root.get());

	// Calculate nullable
	calc_nullable(root_node);

	// Calculate firstpos and lastpos
	calc_first_last_pos(root_node);

	// Calculate followpos for the RegexTree
	calc_followpos(root_node);
}

void AugmentedRegexTree::calc_nullable(AugmentedRegexTree::Node* node)
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

void AugmentedRegexTree::calc_first_last_pos(AugmentedRegexTree::Node* node)
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

void AugmentedRegexTree::calc_followpos(AugmentedRegexTree::Node* node)
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
				static_cast<Node*>(_leaves[lpos])->followpos.insert(fpos);
			}
		}
		calc_followpos(node->left());
		calc_followpos(node->right());
	}
	if (node->is_star())
	{
		for (auto lpos : node->child()->lastpos)
		{
			static_cast<Node*>(_leaves[lpos])->followpos.insert(
				node->child()->firstpos.cbegin(),
				node->child()->firstpos.cend());
		}
		calc_followpos(node->child());
	}
}
