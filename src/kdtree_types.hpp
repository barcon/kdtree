#ifndef KDTREE_TYPES_HPP_
#define KDTREE_TYPES_HPP_

#include "utils.hpp"
#include "logger.hpp"
#include "eilig.hpp"
#include "basis.hpp"
#include "nodes.hpp"

#include <memory>
#include <vector>

namespace kdtree
{

	using Scalar = eilig::Scalar;
	using Matrix = eilig::Matrix;

	using INodePtr = nodes::INodePtr;
	using IBasisPtr = basis::IBasisPtr;

	using NumberNodes = nodes::Number;

	using Nodes = nodes::Nodes;
	using String = utils::String;

	const String headerKDTree = "KDTREE";

	class Leaf;
	using LeafPtr = std::shared_ptr< Leaf >;
	using ConstLeafPtr = std::shared_ptr< const Leaf >;
	using Leaves = std::vector< LeafPtr >;

	class Tree;
	using TreePtr = std::shared_ptr< Tree >;
	using ConstTreePtr = std::shared_ptr< const Tree >;

} //namespace kd

#endif /* KDTREE_TYPES_HPP_ */