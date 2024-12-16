#ifndef KDTREE_TREE_HPP_
#define KDTREE_TREE_HPP_

#include "kdtree_types.hpp"
#include "kdtree_leaf.hpp"

namespace kdtree
{

void PrintTreeRecursive(const String& prefix, const LeafPtr leaf, bool isLeft);
void PrintTree(const TreePtr tree);

TreePtr CreateTree();
TreePtr CreateTree(const Nodes& nodes);

class Tree : public std::enable_shared_from_this< Tree >
{        
	public:
		virtual ~Tree() = default;

		static TreePtr Create();
		TreePtr GetPtr();
		ConstTreePtr GetPtr() const;            

		void MakeTree(const Nodes& nodes);

		INodePtr Search(INodePtr node);
		Nodes   SearchRadius(INodePtr node, Scalar radius);
		Nodes   SearchNumber(INodePtr node, unsigned long long int number);

		INodePtr SearchNaive(INodePtr node);
		Nodes	SearchNaiveRadius(INodePtr node, Scalar radius);
		Nodes	SearchNaiveNumber(INodePtr node, unsigned long long int number);	
		
		LeafPtr GetRoot() const;

		void 	SetBasis(IBasisPtr basis);
		
	private:   
		Tree();		
		
		INodePtr FindMedian(Nodes& nodes, Nodes& left, Nodes& right, std::size_t dim) const;
		Scalar	FindMaxDistance(const Nodes& nodes, INodePtr node, std::size_t& index) const;

		void MakeTreeRecursive(LeafPtr& head, Nodes& nodes, unsigned long long int depth);
		void SearchRecursive(INodePtr node, LeafPtr head, LeafPtr& best, Scalar& best_distance, std::size_t i, std::size_t dim);
		void SearchRecursive(INodePtr node, LeafPtr head, LeafPtr& best, std::size_t i, std::size_t dim, Nodes& found, Scalar radius);
		void SearchRecursive(INodePtr node, LeafPtr head, std::size_t i, std::size_t dim, Nodes& found, NumberNodes numberNodes, Scalar& max_distance, std::size_t& index);

		LeafPtr root_{nullptr};
		Nodes nodes_{};
		IBasisPtr basis_{nullptr};
};

} //namespace kd

#endif /* KDTREE_TREE_HPP_ */