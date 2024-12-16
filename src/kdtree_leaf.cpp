#include "kdtree_leaf.hpp"

namespace kdtree
{
	LeafPtr CreateLeaf()
	{
		return Leaf::Create();
	}
	LeafPtr Leaf::Create()
	{
		class MakeSharedEnabler : public Leaf
		{
		};

		auto res = std::make_shared<MakeSharedEnabler>();

		return res;

	}
	LeafPtr Leaf::GetPtr()
	{
		return this->shared_from_this();
	}
	ConstLeafPtr Leaf::GetPtr() const
	{
		return const_cast<Leaf*>(this)->GetPtr();
	}
	void Leaf::SetNode(INodePtr node)
	{
		node_ = node;
	}
	void Leaf::SetLeft(LeafPtr Leaf)
	{
		left_ = Leaf;
	}
	void Leaf::SetRight(LeafPtr Leaf)
	{
		right_ = Leaf;
	}
	INodePtr Leaf::GetNode()
	{
		return node_;
	}
	LeafPtr Leaf::GetLeft()
	{
		return left_;
	}
	LeafPtr Leaf::GetRight()
	{
		return right_;
	}
} //namespace kd