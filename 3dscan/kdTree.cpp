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
	std::vector<int>* indices = new std::vector<int>();

	indices->resize(points.size() / dim);
	int help = 0;
	for (std::vector<int>::iterator it = indices->begin(); it != indices->end(); ++it)
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

kdTree::Node::Node(std::vector<int>* indices, std::vector<float> &points, int depth, int dim, Node* parent)
{
	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	// assign parent and intialize children
	m_Parent = parent;
	m_LeftChild = NULL;
	m_RightChild = NULL;
	m_Indices = NULL;

	// sort by axis
	std::sort(begin(*indices), end(*indices), [&](size_t a, size_t b) { return points[a + axis] < points[b + axis]; });

	// choose median
	m_Median = points[(indices->operator[](indices->size() / 2)) + axis];

	// set Min and Max
	m_Min = points[indices->operator[](0) + axis];
	m_Max = points[indices->operator[]((indices->size() - 1)) + axis];

	// split indices
	std::vector<int>* indicesLeft = new std::vector<int>();
	std::vector<int>* indicesRight = new std::vector<int>();
	indicesLeft->clear();
	indicesRight->clear();

	for (std::vector<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		if (points[*it + axis] <= m_Median)	indicesLeft->push_back(*it);
		else indicesRight->push_back(*it);
	}

	bool saveIndices = false;
	// construct left and right child
	if (depth <= 1)
	{
		this->m_Indices = indices;
		delete indicesLeft;
		delete indicesRight;
	}
	else
	{
		if (indicesLeft->size() > 1)
		{
			m_LeftChild = new Node(indicesLeft, points, depth - 1, dim, this);
		}
		else
		{
			delete indicesLeft;
			saveIndices = true;
		}
		if (indicesRight->size() > 1)
		{
			if (!saveIndices) delete indices;
			m_RightChild = new Node(indicesRight, points, depth - 1, dim, this);
		}
		else
		{
			delete indicesRight;
			saveIndices = true;
		}
	}

	if (saveIndices) this->m_Indices = indices;

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
	return *m_Indices;
}

std::vector<int> kdTree::Node::reportPoints(int depth, std::vector<float> &points, std::vector<float> &a, std::vector<float> &b, int &dim)
{
	std::vector<int> result;
	result.clear();

	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	// if range query is out of bounds, return empty vector
	if (a[axis] > this->m_Max || b[axis] < this->m_Min)
	{
		return result;
	}

	// if both points are equal/smaller than median, traverse left child
	if (b[axis] <= this->m_Median && this->m_LeftChild != NULL)
	{
		return this->m_LeftChild->reportPoints(depth - 1, points, a, b, dim);
	}

	// if both points are bigger than median, traverse right child
	if (a[axis] > this->m_Median && this->m_RightChild != NULL)
	{
		return this->m_RightChild->reportPoints(depth - 1, points, a, b, dim);
	}

	// range queries a,b with a <= median <b

	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		result = this->m_LeftChild->reportPoints(depth - 1, points, a, b, dim);
	}
	else
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = this->m_Indices->begin(); it != this->m_Indices->end(); ++it)
		{
			if (points[*it + axis] <= this->m_Median)
			{
				// if point fits bounding box in every dimension
				if (testPointInRange(*it, axis, points, a, b, dim) == true)
				{
					result.push_back(*it);
				}
			}
			else break;
		}
	}

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		std::vector<int> resultRight = this->m_RightChild->reportPoints(depth - 1, points, a, b, dim);
		// Combine results of left and right child
		result.reserve(result.size() + resultRight.size()); // preallocate memory
		result.insert(result.end(), resultRight.begin(), resultRight.end());
	}
	else
	{
		// traverse (sorted) indices from back to front until value is smaller than median
		for (std::vector<int>::reverse_iterator it = this->m_Indices->rbegin(); it != this->m_Indices->rend(); ++it)
		{
			if (points[*it + axis] > this->m_Median)
			{
				// if point fits bounding box in every dimension
				if (testPointInRange(*it, axis, points, a, b, dim) == true)
				{
					result.push_back(*it);
				}
			}
			else break;
		}
	}

	// return Indexlist of points in query range
	return result;
}

bool kdTree::Node::testPointInRange(int index, int axis, std::vector<float> &points, std::vector<float> &a, std::vector<float> &b, int &dim)
{

	bool pointInRange = true;
	for (int iterDim = 0; iterDim < dim; ++iterDim)
	{
		// if point is smaller than lower border
		if (a[iterDim] > points[index + iterDim])
		{
			pointInRange = false;
			break;
		}
		// if point is bigger than upper border
		if (b[iterDim] < points[index + iterDim])
		{
			pointInRange = false;
			break;
		}
	}
	return pointInRange;
}


kdTree::Node* kdTree::Node::locatePoint(QVector3D p, int depth , int &dim)
{
	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	if (p[axis] <= this->m_Median)
	{
		if (this->m_LeftChild != NULL)
		{
			return this->m_LeftChild->locatePoint(p, depth - 1, dim);
		}
		else
		{
			return this;
		}
	}
	else
	{
		if (this->m_RightChild != NULL)
		{
			return this->m_RightChild->locatePoint(p, depth - 1, dim);
		}
		else
		{
			return this;
		}
	}
	
}


// Getters

int kdTree::getNumberOfPoints()
{
	return m_Points.size() / m_Dim;
}

std::vector<float> kdTree::getPoints()
{
	return m_Points;
}

// Functions

std::vector<int> kdTree::rangeQuery(QVector3D p1, QVector3D p2)
{
	// assign lower (a) and upper (b) boundary of range query for each dimension (0,1,2,...)
	std::vector<float> a, b;
	a.clear(); a.resize(m_Dim); 
	b.clear(); b.resize(m_Dim);
	for (int iter = 0; iter < m_Dim; ++iter)
	{
		if (p1[iter] > p2[iter])
		{
			b[iter] = p1[iter];
			a[iter] = p2[iter];
		}
		else
		{
			a[iter] = p1[iter];
			b[iter] = p2[iter];
		}
	}
	// recursivly report points
	return this->m_Root->reportPoints(this->m_MaxDepth, this->m_Points, a, b, this->m_Dim);
}

std::vector<int> kdTree::nearestNeighbor(QVector3D p)
{
	Node* location = this->m_Root->locatePoint(p, this->m_MaxDepth, this->m_Dim);
	return std::vector<int>();

}




