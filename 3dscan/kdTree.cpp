#include "kdTree.h"

// Constructors

kdTree::kdTree()
{
}

kdTree::kdTree(std::vector<float> points, int maxDepth, int dim)
{
	m_MaxDepth = maxDepth;
	m_Points = points;
	m_Dim = dim;
	m_Points = points;
	std::vector<int> indices;
	
	indices.resize(points.size() / 3);
	int help = 0;
	for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it)
	{
		*it = help++;
	}
	m_Root = &Node(indices, points, maxDepth, dim, NULL);
}

kdTree::~kdTree()
{
};

kdTree::Node::Node()
{
}

kdTree::Node::Node(std::vector<int> indices, std::vector<float> &points, int depth, int dim, Node* parent)
{
	int axis = depth % dim;
	m_Parent = parent;	

	// sort by axis
	std::sort(begin(indices), end(indices), [&](size_t a, size_t b) { return points[a+axis] < points[b+axis]; });

	// choose median
	m_Median = points[indices[indices.size() / 2]-1];

	// split indices
	std::vector<int> indicesLeft, indicesRight;
	indicesLeft.clear();
	indicesRight.clear();

	for (int iter = 0; iter < indices.size(); iter++)
	{
		if (iter <= indices.size() / 2-1)	indicesLeft.push_back(indices[iter]);
		else indicesRight.push_back(indices[iter]);
	}
	// construct left and right child
	if(depth > 1)
	{
		m_LeftChild = &Node(indicesLeft, points, depth - 1, dim, this);
		m_RightChild = &Node(indicesRight, points, depth - 1, dim, this);
	}

}
kdTree::Node::~Node()
{
}


// Getters

int kdTree::getNumberOfPoints()
{
	return m_Points.size() / 3;
}

std::vector<float> kdTree::getPoints()
{
	return m_Points;
}
