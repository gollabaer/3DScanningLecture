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
	std::vector<int> indices;

	indices.resize(points.size() / 3);
	int help = 0;
	for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it)
	{
		*it = help++ * m_Dim;
	}
		m_Root = new Node(indices, m_Points, m_MaxDepth, m_Dim, NULL);
}

kdTree::~kdTree()
{
};

// Node Class

kdTree::Node::Node()
{
}

kdTree::Node::Node(std::vector<int> indices, std::vector<float> &points, int depth, int dim, Node* parent)
{
	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	// assign parent and intialize children
	m_Parent = parent;
	m_LeftChild = NULL;
	m_RightChild = NULL;

	// sort by axis
	std::sort(begin(indices), end(indices), [&](size_t a, size_t b) { return points[a + axis] < points[b + axis]; });
	m_Indices = indices;

	// choose median
	m_Median = points[(indices[indices.size() / 2]) + axis];

	// set Min and Max
	m_Max = points[indices[0] + axis];
	m_Min = points[indices[(indices.size() - 1)] + axis];

	// split indices
	std::vector<int> indicesLeft, indicesRight;
	indicesLeft.clear();
	indicesRight.clear();

	for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it)
	{
		if (points[*it + axis] <= m_Median)	indicesLeft.push_back(*it);
		else indicesRight.push_back(*it);
	}

	// construct left and right child
	if (depth > 1)
	{
		if (indicesLeft.size() > 1)	m_LeftChild =  new Node(indicesLeft, points, depth - 1, dim, this);
		if (indicesRight.size() > 1) m_RightChild =  new Node(indicesRight, points, depth - 1, dim, this);
	}

}
kdTree::Node::~Node()
{
}

kdTree::Node* kdTree::Node::getParent()
{
	return m_Parent;
}

kdTree::Node* kdTree::Node::getLeftChild()
{
	return m_LeftChild;
}

kdTree::Node* kdTree::Node::getRightChild()
{
	return m_RightChild;
}

int kdTree::Node::getMedian()
{
	return m_Median;
}

float kdTree::Node::getMax()
{
	return m_Max;
}

float kdTree::Node::getMin()
{
	return m_Min;
}

std::vector<int> kdTree::Node::getIndices()
{
	return m_Indices;
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

// Functions

std::vector<int> kdTree::rangeQuery(QVector3D p1, QVector3D p2)
{
	return traverse(m_Root, m_MaxDepth, m_Points, p1, p2);
}

std::vector<int> kdTree::traverse(kdTree::Node* node, int depth, std::vector<float> &points, QVector3D p1, QVector3D p2)
{
	// choose current axis
	int axis = depth % m_Dim; // 0 = x, 1 = y, 2 = z

	// if both points are equal/smaller than median, traverse left child
	if (p1[axis] <= node->getMedian() && p2[axis] <= node->getMedian() && node->getLeftChild() != NULL)
	{
		return traverse(node->getLeftChild(), depth - 1, points, p1, p2);
	}

	// if both points are bigger than median, traverse right child
	if (p1[axis] > node->getMedian() && p2[axis] > node->getMedian() && node->getRightChild() != NULL)
	{
		return traverse(node->getRightChild(), depth - 1, points, p1, p2);
	}

	// assign lower (a) and upper (b) boundary of range query for each dimension (0,1,2,...)
	std::vector<float> a, b;
	for (int iter = 0; iter <= m_Dim; ++iter)
	{
		int pos = (axis + iter) % m_Dim;
		if (p1[pos] > p2[pos])
		{
			b.push_back(p1[pos]);
			a.push_back(p2[pos]);
		}
		else
		{
			a.push_back(p1[pos]);
			b.push_back(p2[pos]);
		}
	}

	// all combinations for range query a,b with a <= median <b
	std::vector<int> result;
	result.clear();

	// check left child and add points to result
	if (node->getLeftChild() != NULL)
	{
		result = traverse(node->getLeftChild(), depth - 1, points, p1, p2);
	}
	else
	{
		std::vector<int> indices = node->getIndices();
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it)
		{
			if (points[*it + axis] <= node->getMedian())
			{
				// if point fits bounding box in every dimension
				if (testPointInBoundingBox(*it, axis, points, a, b) == true)
				{
					result.push_back(*it);
				}
			}
			else break;
		}
	}

	// check right child and add points to result
	if (node->getRightChild() != NULL)
	{
		std::vector<int> resultRight = traverse(node->getRightChild(), depth - 1, points, p1, p2);
		// Combine results of left and right child
		result.reserve(result.size() + resultRight.size()); // preallocate memory
		result.insert(result.end(), resultRight.begin(), resultRight.end());
	}
	else
	{
		std::vector<int> indices = node->getIndices();
		// traverse (sorted) indices from back to front until value is smaller than median
		for (std::vector<int>::reverse_iterator it = indices.rbegin(); it != indices.rend(); ++it)
		{
			if (points[*it + axis] > node->getMedian())
			{
				// if point fits bounding box in every dimension
				if (testPointInBoundingBox(*it, axis, points, a, b) == true)
				{
					result.push_back(*it);
				}
			}
			else break;
		}
	}

	// return Indixlist of points in query range
	return result;
}

bool kdTree::testPointInBoundingBox(int index, int axis, std::vector<float> &points, std::vector<float> &a, std::vector<float> &b)
{

	bool pointInBoundingBox = true;
	for (int iterDim = 0; iterDim <= m_Dim; ++iterDim)
	{
		int pos = (axis + iterDim) % m_Dim;
		// if point is smaller than lower border
		if (a[iterDim] > points[index + pos])
		{
			pointInBoundingBox = false;
			break;
		}
		// if point is bigger than upper border
		if (b[iterDim] < points[index + pos])
		{
			pointInBoundingBox = false;
			break;
		}
	}
	return pointInBoundingBox;
}

