#include "Tree3d.h"
#include <assert.h> 

const int DIM = 3;

// Constructors

Tree3d::Tree3d()
{
}

Tree3d::Tree3d(std::vector<Point3d> &points, int maxDepth)
{
	// assign member variables 
	m_MaxDepth = maxDepth;
	m_Points = points;
	// intialize indices vector for root node
	std::vector<int>* indices = new std::vector<int>();
	indices->resize(points.size());
	int tmp = 0;
	for (std::vector<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		*it = tmp++;
	}
	// recursivly build tree
	m_Root = new Node(indices, m_Points, m_MaxDepth, NULL);
}

Tree3d::~Tree3d()
{
	// delete this->m_Root;
};

// Node Class

Tree3d::Node::Node()
{
}

Tree3d::Node::Node(std::vector<int>* indices, std::vector<Point3d> &points, int depth, Node* parent)
{
	// choose current axis
	this->m_Axis = depth % DIM; // 0 = x, 1 = y, 2 = z

	// assign parent and intialize children
	m_Parent = parent;
	m_LeftChild = NULL;
	m_RightChild = NULL;
	m_Indices = NULL;

	// sort by axis
	std::sort(begin(*indices), end(*indices), [&](size_t a, size_t b) { return points[a][m_Axis] < points[b][m_Axis]; });

	// choose median
	int medianIndex = 0;
	if (indices->size() % 2 == 0)
	{
		medianIndex = indices->size() / 2 - 1;
	}
	else
	{
		medianIndex = indices->size() / 2;
	}
	m_Median = points[indices->operator[](medianIndex)][m_Axis];

	// set Min and Max
	m_Min = points[indices->operator[](0)][m_Axis];
	m_Max = points[indices->operator[]((indices->size() - 1))][m_Axis];

	if (depth <= 1 || indices->size() <= 1)
	{
		// if maximum depth is reached or node just contains just one point save indices and don't build childnodes
		this->m_Indices = indices;
		return;
	}

	// trueMedianIndex is the index of the 'last' occurence of the median value in the sorted index list in case of multiple points with the median value
	int trueMedianIndex = indices->size() - 1;

	// if right side of indices list contains at least one point build right child
	if (m_Max > m_Median)
	{
		std::vector<int>* indicesRight = new std::vector<int>();
		indicesRight->clear();

		// find trueMedianIndex
		for (int index = medianIndex; index <= indices->size() - 1; ++index)
		{
			if (points[indices->operator[](index)][m_Axis] > m_Median)
			{
				// as soon as the point larger than the median is found, the predecessor is defined as the trueMedianIndex
				trueMedianIndex = index - 1;
				break;
			}
		}

		// copy all indices between the trueMedianIndex + 1 and the last index into indicesRight
		indicesRight->resize(indices->size() - trueMedianIndex - 1);
		std::copy(indices->begin() + trueMedianIndex + 1, indices->end(), indicesRight->begin());

		// build right child node
		m_RightChild = new Node(indicesRight, points, depth - 1, this);
	}

	// left child can't be empty since size > 1 and Median is part of left child
	std::vector<int>* indicesLeft = new std::vector<int>();
	indicesLeft->clear();

	// copy all indices between the first index and the trueMedianIndex into indicesLeft
	indicesLeft->resize(trueMedianIndex + 1);
	std::copy(indices->begin(), indices->begin() + trueMedianIndex + 1, indicesLeft->begin());

	// build left child node
	m_LeftChild = new Node(indicesLeft, points, depth - 1, this);

	// delet indices since they are all stored in the child nodes
	delete indices;
}

Tree3d::Node::~Node()
{
	//delete this->m_Indices;
	//delete this->m_LeftChild;
	//delete this->m_RightChild;
}

Tree3d::Node* Tree3d::Node::getParent()
{
	return m_Parent;
}

Tree3d::Node* Tree3d::Node::getLeftChild()
{
	return m_LeftChild;
}

Tree3d::Node* Tree3d::Node::getRightChild()
{
	return m_RightChild;
}

int Tree3d::Node::getMedian()
{
	return m_Median;
}

float Tree3d::Node::getMax()
{
	return m_Max;
}

float Tree3d::Node::getMin()
{
	return m_Min;
}

std::vector<int> Tree3d::Node::getIndices()
{
	if (m_Indices != NULL)
		return *m_Indices;
	else
		return std::vector<int> ();

}

bool Tree3d::Node::isLeaf()
{
	if (this->m_Indices != NULL) return true;
	else return false;
}

std::vector<int> Tree3d::Node::rangeQuery(std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary)
{
	// b ">=" a is already asserted in the range query function!
	std::vector<int> result;
	result.clear();

	// if range query is out of bounds, return empty vector
	if (lowerBoundary[m_Axis] > this->m_Max || upperBoundary[m_Axis] < this->m_Min)
	{
		return result;
	}

	// range queries a,b with min <= a <= median <b <= max

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		result = this->m_RightChild->rangeQuery(points, lowerBoundary, upperBoundary);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		std::vector<int> resultLeft = this->m_LeftChild->rangeQuery(points, lowerBoundary, upperBoundary);
		// Combine results of left and right child
		result.reserve(result.size() + resultLeft.size()); // preallocate memory
		result.insert(result.end(), resultLeft.begin(), resultLeft.end());
	}
	// if the left child is NULL the node has to be a leaf, so traverse points and return fitting ones
	else 
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = this->m_Indices->begin(); it != this->m_Indices->end(); ++it)
		{
			// if point fits bounding box in every DIMension
			if (pointIsInRange(*it, points, lowerBoundary, upperBoundary))
			{
				result.push_back(*it);
			}
		}
	}
	// return Indexlist of points in query range
	return result;
}

bool Tree3d::Node::pointIsInRange(int index, std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary)
{
	bool pointInRange = true;
	// check for each DIMension if point is inbetween lower and upper border
	for (int iterDIM = 0; iterDIM < DIM; ++iterDIM)
	{
		// if point is smaller than lower border
		if (lowerBoundary[iterDIM] > points[index][iterDIM])
		{
			pointInRange = false;
			break;
		}
		// if point is bigger than upper border
		if (upperBoundary[iterDIM] < points[index][iterDIM])
		{
			pointInRange = false;
			break;
		}
	}
	return pointInRange;
}

std::vector<int> Tree3d::Node::radiusQuery(std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary, Point3d queryPoint, float radius)
{
	// b ">=" a is already asserted in the radius query function!
	std::vector<int> result;
	result.clear();

	// if radius query is out of bounds, return empty vector
	if (lowerBoundary[m_Axis] > this->m_Max || upperBoundary[m_Axis] < this->m_Min)
	{
		return result;
	}

	// radius queries a,b with min <= a <= median <b <= max

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		result = this->m_RightChild->radiusQuery(points, lowerBoundary, upperBoundary, queryPoint, radius);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		std::vector<int> resultLeft = this->m_LeftChild->radiusQuery(points, lowerBoundary, upperBoundary, queryPoint, radius);
		// Combine results of left and right child
		result.reserve(result.size() + resultLeft.size()); // preallocate memory
		result.insert(result.end(), resultLeft.begin(), resultLeft.end());
	}
	// if the left child is NULL the node has to be a leaf, so traverse points and return fitting ones
	else
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = this->m_Indices->begin(); it != this->m_Indices->end(); ++it)
		{

			// if point is in radius distance from query point
			if (sqDistance3d(points[*it], queryPoint) <= radius*radius)
			{
				result.push_back(*it);
			}
		}
	}
	// return Indexlist of points in query range
	return result;
}

void Tree3d::Node::nearestNeighbour(Point3d queryPoint, float &currentRange, int &index, std::vector<Point3d> &points){

	float pointm_AxisValues = queryPoint[m_Axis];

	//report nearest neighbour in leaf
	if (isLeaf()) {

		float t_dist =  sqDistance3d(queryPoint, points[m_Indices->operator[](0)]);
		int t_index = m_Indices->operator[](0);

		//find closest point in node 
		for (int i = 1; i < m_Indices->size(); i++)
		{
			float t_t_dist = sqDistance3d(queryPoint, points[m_Indices->operator[](i)]);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				t_index = m_Indices->operator[](i);
			}
		}
		if (currentRange > t_dist){
			currentRange = t_dist;
			index = t_index;
			return;
		}
	}

	if (pointm_AxisValues < m_Median)
	{
		if (m_LeftChild != NULL)
			m_LeftChild->nearestNeighbour(queryPoint, currentRange, index, points);

		if (pointm_AxisValues + currentRange > m_Median && m_RightChild != NULL)
			m_RightChild->nearestNeighbour(queryPoint, currentRange, index, points);
	}
	else
	{
		if (m_RightChild != NULL)
			m_RightChild->nearestNeighbour(queryPoint, currentRange, index, points);

		if (pointm_AxisValues - currentRange < m_Median && m_LeftChild != NULL)
			m_LeftChild->nearestNeighbour(queryPoint, currentRange, index, points);
	}
}


// Getters

int Tree3d::getNumberOfPoints()
{
	return m_Points.size() / DIM;
}

std::vector<Point3d> Tree3d::getPoints()
{
	return m_Points;
}

// Functions

std::vector<int> Tree3d::rangeQuery(Point3d p1, Point3d p2)
{
	// assign lower and upper boundary of range query for each DIMension (0,1,2,...)
	Point3d lowerBoundary, upperBoundary;

	for (int iter = 0; iter < DIM; ++iter)
	{
		if (p1[iter] > p2[iter])
		{
			upperBoundary[iter] = p1[iter];
			lowerBoundary[iter] = p2[iter];
		}
		else
		{
			lowerBoundary[iter] = p1[iter];
			upperBoundary[iter] = p2[iter];
		}
	}
	// recursivly report points
	return this->m_Root->rangeQuery(this->m_Points, lowerBoundary, upperBoundary);
}

std::vector<int> Tree3d::radiusQuery(Point3d queryPoint, float radius)
{
	Point3d lowerBoundary = queryPoint - Point3d(radius, radius, radius);
	Point3d upperBoundary = queryPoint + Point3d(radius, radius, radius);

	return this->m_Root->radiusQuery(this->m_Points, lowerBoundary, upperBoundary, queryPoint, radius);
}

int Tree3d::nearestNeighbour(Point3d p)
{
	// location is not always a leaf node and has indices
	int ind = -1;

	float dist = 1000000000.0f;

	m_Root->nearestNeighbour(p, dist, ind, m_Points);
	return ind;
}

