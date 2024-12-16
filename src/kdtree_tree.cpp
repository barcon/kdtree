#include "kdtree_tree.hpp"
#include <limits>

namespace kdtree
{
	void PrintTreeRecursive(const String& prefix, const LeafPtr leaf, bool isLeft)
	{
		if (leaf != nullptr)
		{
			std::cout << prefix;
			std::cout << (isLeft ? "<--- " : ">--- ");

			std::cout << "(" << leaf->GetNode()->GetPoint()(0) << ","
				<< leaf->GetNode()->GetPoint()(1) << ","
				<< leaf->GetNode()->GetPoint()(2) << ")"
				<< std::endl;

			PrintTreeRecursive(prefix + (isLeft ? "<--- " : "    "), leaf->GetLeft(), true);
			PrintTreeRecursive(prefix + (isLeft ? "<    " : "    "), leaf->GetRight(), false);
		}
	}
	void PrintTree(const TreePtr tree)
	{
		PrintTreeRecursive("", tree->GetRoot(), false);
	}

	TreePtr CreateTree()
	{
		return Tree::Create();
	}
	TreePtr CreateTree(const Nodes& nodes)
	{
		auto res = Tree::Create();

		res->MakeTree(nodes);

		return res;
	}
	Tree::Tree()
	{
		basis_ = basis::CreateBasisCartesian();
	}
	TreePtr Tree::Create()
	{
		class MakeSharedEnabler : public Tree
		{
		};

		auto res = std::make_shared<MakeSharedEnabler>();

		return res;
	}
	TreePtr Tree::GetPtr()
	{
		return this->shared_from_this();
	}
	ConstTreePtr Tree::GetPtr() const
	{
		return const_cast<Tree*>(this)->GetPtr();
	}
	void Tree::MakeTree(const Nodes& nodes)
	{
		LeafPtr head = CreateLeaf();

		if (nodes.size() == 0)
		{
			logger::Error(headerKDTree, "Number of Nodes must be > 0");
			return;
		}

		root_ = head;
		nodes_ = nodes;

		MakeTreeRecursive(head, nodes_, 0);
	}
	INodePtr Tree::Search(INodePtr node)
	{
		LeafPtr head = root_;
		LeafPtr best = nullptr;
		Scalar	best_distance = std::numeric_limits<Scalar>::max();

		if (node == nullptr)
		{
			return nullptr;
		}

		SearchRecursive(node, head, best, best_distance, 0, basis_->GetDim());

		if (best == nullptr)
		{
			std::cout << "Not found" << std::endl;
		}

		return best->GetNode();
	}
	Nodes Tree::SearchRadius(INodePtr node, Scalar radius)
	{
		LeafPtr head = root_;
		LeafPtr best = nullptr;
		Nodes res;

		if (node == nullptr)
		{
			return res;
		}

		if (radius < 0.)
		{
			return res;
		}

		SearchRecursive(node, head, best, 0, basis_->GetDim(), res, radius);

		return res;
	}
	Nodes Tree::SearchNumber(INodePtr node, unsigned long long int number)
	{
		LeafPtr head = root_;
		LeafPtr best = nullptr;
		Nodes res;

		Scalar max_distance = 0.;
		std::size_t index = 0;

		if (node == nullptr)
		{
			return res;
		}

		if (number == 0)
		{
			return res;
		}

		SearchRecursive(node, head, 0, basis_->GetDim(), res, number, max_distance, index);

		return res;
	}
	INodePtr Tree::SearchNaive(INodePtr node)
	{
		Scalar	aux1 = 0;
		Scalar	aux2 = 0;
		INodePtr	res = nullptr;

		if (node == nullptr)
		{
			return nullptr;
		}

		if (nodes_.size() != 0) {
			aux1 = basis_->DistanceSquared(node->GetPoint(), nodes_[0]->GetPoint());
			res = nodes_[0];

			for (auto& it : nodes_) {
				aux2 = basis_->DistanceSquared(node->GetPoint(), it->GetPoint());

				if (aux2 == 0.) {
					res = it;
					break;
				}
				else if (aux1 > aux2) {
					aux1 = aux2;
					res = it;
				}
			}
		}

		return res;
	}
	Nodes Tree::SearchNaiveNumber(INodePtr node, unsigned long long int number)
	{
		Scalar	distance = 0;
		Scalar	max_distance = 0;
		Nodes	found;
		std::size_t index = 0;

		if (node == nullptr)
		{
			return found;
		}

		if (number == 0)
		{
			return found;
		}

		if (nodes_.size() != 0) {

			for (auto& it : nodes_) {
				if (found.size() < number)
				{
					found.push_back(it);
					max_distance = FindMaxDistance(found, node, index);
				}
				else
				{
					distance = basis_->DistanceSquared(it->GetPoint(), node->GetPoint());

					if (distance < max_distance)
					{
						found[index] = it;
						max_distance = FindMaxDistance(found, node, index);
					}
				}
			}

		}

		return found;
	}
	Nodes Tree::SearchNaiveRadius(INodePtr node, Scalar radius)
	{
		Scalar	distance = 0;
		Nodes	found;

		if (node == nullptr)
		{
			return found;
		}

		if (radius < 0.)
		{
			return found;
		}

		if (nodes_.size() != 0) {
			for (auto& it : nodes_)
			{
				distance = basis_->DistanceSquared(it->GetPoint(), node->GetPoint());
				if (distance <= (radius * radius))
				{
					found.push_back(it);
				}
			}

		}

		return found;
	}
	LeafPtr Tree::GetRoot() const
	{
		return root_;
	}
	void Tree::SetBasis(IBasisPtr basis)
	{
		if (basis == nullptr)
		{
			logger::Error(headerKDTree, "Basis pointer is nullptr");
			return;
		}

		basis_ = basis;
	}
	INodePtr Tree::FindMedian(Nodes& nodes, Nodes& left, Nodes& right, std::size_t dim) const
	{
		INodePtr median;
		std::size_t size = nodes.size();
		unsigned int med = static_cast<unsigned int>(std::ceil(size / 2.));
		unsigned int count = 0;

		if (size == 1)
			return nodes[0];

		std::sort(nodes.begin(),
			nodes.end(),
			[&](INodePtr a, INodePtr b)
			{
				return a->GetPoint()(dim) < b->GetPoint()(dim);
			}
		);

		for (auto it : nodes)
		{
			if (count < med)
				left.push_back(it);
			else
				right.push_back(it);

			++count;
		}

		median = left.back();
		left.pop_back();

		return median;
	}
	Scalar	Tree::FindMaxDistance(const Nodes& nodes, INodePtr node, std::size_t& index) const
	{
		Scalar distance = 0.;
		Scalar max_distance = 0.;

		for (std::size_t j = 0; j < nodes.size(); ++j)
		{
			distance = basis_->DistanceSquared(nodes[j]->GetPoint(), node->GetPoint());

			if (distance > max_distance)
			{
				max_distance = distance;
				index = j;
			}
		}

		return max_distance;
	}
	void Tree::MakeTreeRecursive(LeafPtr& head, Nodes& nodes, unsigned long long int depth)
	{
		if (nodes.size() != 0)
		{
			auto k = basis_->GetDim();
			unsigned long long int dim = depth % k;

			Nodes left;
			Nodes right;
			INodePtr median = FindMedian(nodes, left, right, dim);

			head->SetNode(median);

			LeafPtr leftLeaf = CreateLeaf();
			LeafPtr rightLeaf = CreateLeaf();

			MakeTreeRecursive(leftLeaf, left, depth + 1);
			if (!left.empty())
				head->SetLeft(leftLeaf);

			MakeTreeRecursive(rightLeaf, right, depth + 1);
			if (!right.empty())
				head->SetRight(rightLeaf);
		}
	}
	void Tree::SearchRecursive(INodePtr node, LeafPtr head, LeafPtr& best, Scalar& best_distance, std::size_t i, std::size_t dim)
	{
		Scalar distance;
		Scalar dx;
		Scalar dx2;

		if (head == nullptr)
		{
			return;
		}

		distance = basis_->DistanceSquared(head->GetNode()->GetPoint(), node->GetPoint());
		dx = head->GetNode()->GetPoint()(i) - node->GetPoint()(i);
		dx2 = dx * dx;

		if (distance < best_distance)
		{
			best_distance = distance;
			best = head;
		}

		if (utils::math::IsAlmostEqual(distance, 0., 2))
		{
			return;
		}

		if (++i >= dim)
		{
			i = 0;
		}

		SearchRecursive(node, dx > 0 ? head->GetLeft() : head->GetRight(), best, best_distance, i, dim);

		if (dx2 >= best_distance)
		{
			return;
		}

		SearchRecursive(node, dx > 0 ? head->GetRight() : head->GetLeft(), best, best_distance, i, dim);
	}
	void Tree::SearchRecursive(INodePtr node, LeafPtr head, LeafPtr& best, std::size_t i, std::size_t dim, Nodes& found, Scalar radius)
	{
		Scalar distance;
		Scalar dx;
		Scalar dx2;

		if (head == nullptr)
		{
			return;
		}

		distance = basis_->DistanceSquared(head->GetNode()->GetPoint(), node->GetPoint());
		dx = head->GetNode()->GetPoint()(i) - node->GetPoint()(i);
		dx2 = dx * dx;

		if (distance <= radius * radius)
		{
			best = head;
			found.push_back(head->GetNode());
		}

		if (++i >= dim)
		{
			i = 0;
		}

		SearchRecursive(node, dx > 0 ? head->GetLeft() : head->GetRight(), best, i, dim, found, radius);

		if (dx2 > radius * radius)
		{
			return;
		}

		SearchRecursive(node, dx > 0 ? head->GetRight() : head->GetLeft(), best, i, dim, found, radius);
	}
	void Tree::SearchRecursive(INodePtr node, LeafPtr head, std::size_t i, std::size_t dim, Nodes& found, NumberNodes numberNodes, Scalar& max_distance, std::size_t& index)
	{
		Scalar distance;
		Scalar dx;
		Scalar dx2;

		if (head == nullptr)
		{
			return;
		}

		distance = basis_->DistanceSquared(head->GetNode()->GetPoint(), node->GetPoint());
		dx = head->GetNode()->GetPoint()(i) - node->GetPoint()(i);
		dx2 = dx * dx;

		if (found.size() < numberNodes)
		{
			found.push_back(head->GetNode());
			max_distance = FindMaxDistance(found, node, index);
		}
		else
		{
			if (distance < max_distance)
			{
				found[index] = head->GetNode();
				max_distance = FindMaxDistance(found, node, index);
			}
		}

		if (++i >= dim)
		{
			i = 0;
		}

		SearchRecursive(node, dx > 0 ? head->GetLeft() : head->GetRight(), i, dim, found, numberNodes, max_distance, index);

		if (dx2 > max_distance)
		{
			return;
		}

		SearchRecursive(node, dx > 0 ? head->GetRight() : head->GetLeft(), i, dim, found, numberNodes, max_distance, index);
	}
} //namespace kd