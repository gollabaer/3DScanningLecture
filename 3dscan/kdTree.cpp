#include "kdTree.h"
#include <assert.h> 

// Constructors

kdTree::kdTree()
{
}

kdTree::kdTree(std::vector<float> points, int maxDepth, int dim)
{
	// assign member variables 
	m_MaxDepth = maxDepth;
	m_Points = points;
	m_Dim = dim;
	// intialize indices vector for root node
	std::vector<int>* indices = new std::vector<int>();
	indices->resize(points.size() / dim);
	int tmp = 0;
	for (std::vector<int>::iterator it = indices->begin(); it != indices->end(); ++it)
	{
		*it = tmp++ * m_Dim;
	}
	// recursivly build tree
	m_Root = new Node(indices, m_Points, m_MaxDepth, m_Dim, NULL);
}

kdTree::~kdTree()
{
	// delete this->m_Root;
};

// Node Class

kdTree::Node::Node()
{
}

kdTree::Node::Node(std::vector<int>* indices, std::vector<float> &points, int depth, int dim, Node* parent)
{
	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z
	m_axis = axis;
	// assign parent and intialize children
	m_Parent = parent;
	m_LeftChild = NULL;
	m_RightChild = NULL;
	m_Indices = NULL;

	// sort by axis
	std::sort(begin(*indices), end(*indices), [&](size_t a, size_t b) { return points[a + axis] < points[b + axis]; });

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
	m_Median = points[(indices->operator[](medianIndex)) + axis];

	// set Min and Max
	m_Min = points[indices->operator[](0) + axis];
	m_Max = points[indices->operator[]((indices->size() - 1)) + axis];

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
			if (points[indices->operator[](index) + axis] > m_Median)
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
		m_RightChild = new Node(indicesRight, points, depth - 1, dim, this);
	}

	// left child can't be empty since size > 1 and Median is part of left child
	std::vector<int>* indicesLeft = new std::vector<int>();
	indicesLeft->clear();

	// copy all indices between the first index and the trueMedianIndex into indicesLeft
	indicesLeft->resize(trueMedianIndex + 1);
	std::copy(indices->begin(), indices->begin() + trueMedianIndex + 1, indicesLeft->begin());

	// build left child node
	m_LeftChild = new Node(indicesLeft, points, depth - 1, dim, this);

	// delet indices since they are all stored in the child nodes
	delete indices;
}

kdTree::Node::~Node()
{
	//delete this->m_Indices;
	//delete this->m_LeftChild;
	//delete this->m_RightChild;
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

void kdTree::Node::NearesNeighbour(std::vector<float> queryPoint, float &currentRange,int &index, std::vector<float> &points){
	
	float pointAxisValues = queryPoint[m_axis];

	//report nearest neighbour in leaf
	if (isLeaf()) {
		std::vector<float> t_vec =  kdTree::indexToVector(m_Indices->at(0), points);
		float t_dist = kdTree::squaredEuclidianDistance(queryPoint, t_vec);
		int t_index = m_Indices->at(0);

		//find closest point in node 
		for (int i = 1; i < m_Indices->size(); i++)
		{
			t_vec = indexToVector(m_Indices->at(i), points);
			float t_t_dist = squaredEuclidianDistance(queryPoint, t_vec);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				t_index = m_Indices->at(i);
			}
		}
		if (currentRange > t_dist){
			currentRange = t_dist;
			index = t_index;
			return;
		}
	}

	if (pointAxisValues < m_Median){
	
		if (m_LeftChild != NULL)
			m_LeftChild->NearesNeighbour(queryPoint, currentRange,index, points);

		if (pointAxisValues + currentRange > m_Median && m_RightChild != NULL)
			m_RightChild->NearesNeighbour(queryPoint, currentRange, index, points);
	}
	else{
		if (m_RightChild != NULL)
		m_RightChild->NearesNeighbour(queryPoint, currentRange, index, points);

		if (pointAxisValues - currentRange < m_Median && m_LeftChild != NULL)
			m_LeftChild->NearesNeighbour(queryPoint, currentRange, index, points);
	}
	

}

std::vector<int> kdTree::Node::reportPoints(int depth, std::vector<float> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary, int &dim)
{
	// TODO: check that b ">" a !this is already asserted in the range query function!
	std::vector<int> result;
	result.clear();

	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	// if range query is out of bounds, return empty vector
	if (lowerBoundary[axis] > this->m_Max || upperBoundary[axis] < this->m_Min)
	{
		return result;
	}

	// range queries a,b with min <= a <= median <b <= max

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		result = this->m_RightChild->reportPoints(depth - 1, points, lowerBoundary, upperBoundary, dim);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		std::vector<int> resultLeft = this->m_LeftChild->reportPoints(depth - 1, points, lowerBoundary, upperBoundary, dim);
		// Combine results of left and right child
		result.reserve(result.size() + resultLeft.size()); // preallocate memory
		result.insert(result.end(), resultLeft.begin(), resultLeft.end());
	}
	else
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = this->m_Indices->begin(); it != this->m_Indices->end(); ++it)
		{
			// if point fits bounding box in every dimension
			if (testPointInRange(*it, axis, points, lowerBoundary, upperBoundary, dim) == true)
			{
				result.push_back(*it);
			}
		}
	}
	// return Indexlist of points in query range
	return result;
}

bool kdTree::Node::testPointInRange(int index, int axis, std::vector<float> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary, int &dim)
{
	bool pointInRange = true;
	// check for each dimension if point is inbetween lower and upper border
	for (int iterDim = 0; iterDim < dim; ++iterDim)
	{
		// if point is smaller than lower border
		if (lowerBoundary[iterDim] > points[index + iterDim])
		{
			pointInRange = false;
			break;
		}
		// if point is bigger than upper border
		if (upperBoundary[iterDim] < points[index + iterDim])
		{
			pointInRange = false;
			break;
		}
	}
	return pointInRange;
}


kdTree::Node* kdTree::Node::locatePoint(std::vector<float> p, int depth, int &dim)
{
	// choose current axis
	int axis = depth % dim; // 0 = x, 1 = y, 2 = z

	if (p[axis] <= this->m_Median)
	{
		if (this->m_LeftChild != NULL)
		{
			// if axis coordinate is smaller/equal than median locate point in left child
			return this->m_LeftChild->locatePoint(p, depth - 1, dim);
		}
		else
		{
			// if left child is NULL point location is this node
			return this;
		}
	}
	else
	{
		if (this->m_RightChild != NULL)
		{
			// if axis coordinate is bigger than median locate point in right child
			return this->m_RightChild->locatePoint(p, depth - 1, dim);
		}
		else
		{
			// if right child is NULL point location is this node
			return this;
		}
	}

}

bool kdTree::Node::isLeaf()
{
	if (this->m_Indices != NULL) return true;
	else return false;
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

std::vector<int> kdTree::rangeQuery(std::vector<float> p1, std::vector<float> p2)
{
	// assign lower and upper boundary of range query for each dimension (0,1,2,...)
	std::vector<float> lowerBoundary, upperBoundary;
	lowerBoundary.clear(); lowerBoundary.resize(m_Dim);
	upperBoundary.clear(); upperBoundary.resize(m_Dim);
	for (int iter = 0; iter < m_Dim; ++iter)
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
	return this->m_Root->reportPoints(this->m_MaxDepth, this->m_Points, lowerBoundary, upperBoundary, this->m_Dim);
}


std::vector<float> kdTree::indexToVector(int i, std::vector<float> &points)
{
	return std::vector<float>(points.begin() + i,
		points.begin() + i + (TREE_DIM));
}

int kdTree::nearestNeighbor(std::vector<float> p)
{
	// location is not always a leaf node and has indices
	int ind = -1;

	float dist = 1000000000.0f;

	m_Root->NearesNeighbour(p, dist, ind, m_Points);
	return ind;
}

int kdTree::closestPointFromLocation(kdTree::Node* location, std::vector<float> p)
{
	// location is not a leaf node if the value is bigger than the median, but no right child exitsts
	if (!location->isLeaf())
	{
		return closestPointFromLocation(location->getLeftChild(), p);
	}
	else
	{
		std::vector<int> ind_points = location->getIndices();

		std::vector<float> t_vec = indexToVector(ind_points[0], m_Points);
		float t_dist = squaredEuclidianDistance(p, t_vec);
		int index = ind_points[0];

		for (int i = 1; i < ind_points.size(); i++){
			t_vec = indexToVector(ind_points[i],m_Points);
			float t_t_dist = squaredEuclidianDistance(p, t_vec);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				index = ind_points[i];
			}
		}

		std::vector<float> q1 = p, q2 = p;

		t_dist = std::sqrt(t_dist);
		for (int i = 0; i < this->m_Dim; i++){
			q1[i] += t_dist;
			q2[i] -= t_dist;
		}


		std::vector<int> ind_points_range = rangeQuery(q1, q2);

		t_vec = indexToVector(ind_points_range[0],m_Points);
		t_dist = squaredEuclidianDistance(p, t_vec);
		index = ind_points_range[0];


		for (int i = 1; i < ind_points_range.size(); i++)
		{
			t_vec = indexToVector(ind_points_range[i],m_Points);
			float t_t_dist = squaredEuclidianDistance(p, t_vec);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				index = ind_points_range[i];
			}
		}

		return index;
	}
}

double kdTree::squaredEuclidianDistance(std::vector<float> &p1, std::vector<float> &p2)
{
	assert(p1.size() == p2.size());
	double result = 0;
	for (int iter = 0; iter <= p1.size() - 1; iter++)
	{
		result += (p1[iter] - p2[iter]) * (p1[iter] - p2[iter]);
	}
	return result;
}



