#ifndef KDTREE_LEAF_HPP_
#define KDTREE_LEAF_HPP_

#include "kdtree_types.hpp"

namespace kdtree
{
	LeafPtr CreateLeaf();

	class Leaf : public std::enable_shared_from_this< Leaf >
	{
	public:
		virtual ~Leaf() = default;

		static LeafPtr Create();
		LeafPtr GetPtr();
		ConstLeafPtr GetPtr() const;

		void SetNode(INodePtr node);
		void SetLeft(LeafPtr Leaf);
		void SetRight(LeafPtr Leaf);

		INodePtr GetNode();
		LeafPtr GetLeft();
		LeafPtr GetRight();

	private:
		Leaf() = default;

		INodePtr node_{ nullptr };
		LeafPtr left_{ nullptr };
		LeafPtr right_{ nullptr };
	};
} //namespace kd

#endif /* KDTREE_LEAF_HPP_ */