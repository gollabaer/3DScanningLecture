#include "Tree3d.h"
#include <assert.h> 

const int DIM = 3;

// Constructors

Tree3d::Tree3d()
{
}

Tree3d::Tree3d(std::vector<Point3d> points, int maxDepth)
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
	int axis = depth % DIM; // 0 = x, 1 = y, 2 = z

	// assign parent and intialize children
	m_Parent = parent;
	m_LeftChild = NULL;
	m_RightChild = NULL;
	m_Indices = NULL;

	// sort by axis
	std::sort(begin(*indices), end(*indices), [&](size_t a, size_t b) { return points[a][axis] < points[b][axis]; });

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
	m_Median = points[indices->operator[](medianIndex)][axis];

	// set Min and Max
	m_Min = points[indices->operator[](0)][axis];
	m_Max = points[indices->operator[]((indices->size() - 1))][axis];

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
			if (points[indices->operator[](index)][axis] > m_Median)
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
	return *m_Indices;
}

std::vector<int> Tree3d::Node::reportPoints(int depth, std::vector<Point3d> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary)
{
	// TODO: check that b ">" a !this is already asserted in the range query function!
	std::vector<int> result;
	result.clear();

	// choose current axis
	int axis = depth % DIM; // 0 = x, 1 = y, 2 = z

	// if range query is out of bounds, return empty vector
	if (lowerBoundary[axis] > this->m_Max || upperBoundary[axis] < this->m_Min)
	{
		return result;
	}

	// range queries a,b with min <= a <= median <b <= max

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		result = this->m_RightChild->reportPoints(depth - 1, points, lowerBoundary, upperBoundary);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		std::vector<int> resultLeft = this->m_LeftChild->reportPoints(depth - 1, points, lowerBoundary, upperBoundary);
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
			if (testPointInRange(*it, axis, points, lowerBoundary, upperBoundary) == true)
			{
				result.push_back(*it);
			}
		}
	}
	// return Indexlist of points in query range
	return result;
}

bool Tree3d::Node::testPointInRange(int index, int axis, std::vector<Point3d> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary)
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


Tree3d::Node* Tree3d::Node::locatePoint(Point3d p, int depth)
{
	// choose current axis
	int axis = depth % DIM; // 0 = x, 1 = y, 2 = z

	if (p[axis] <= this->m_Median)
	{
		if (this->m_LeftChild != NULL)
		{
			// if axis coordinate is smaller/equal than median locate point in left child
			return this->m_LeftChild->locatePoint(p, depth - 1);
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
			return this->m_RightChild->locatePoint(p, depth - 1);
		}
		else
		{
			// if right child is NULL point location is this node
			return this;
		}
	}

}

bool Tree3d::Node::isLeaf()
{
	if (this->m_Indices != NULL) return true;
	else return false;
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
	std::vector<float> lowerBoundary, upperBoundary;
	lowerBoundary.clear(); lowerBoundary.resize(DIM);
	upperBoundary.clear(); upperBoundary.resize(DIM);
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
	return this->m_Root->reportPoints(this->m_MaxDepth, this->m_Points, lowerBoundary, upperBoundary);
}


Point3d Tree3d::indexToPoint(int i)
{
	return *(this->m_Points.begin() + i);
}

int Tree3d::nearestNeighbor(Point3d p)
{
	// location is not always a leaf node and has indices
	Node* location = this->m_Root->locatePoint(p, this->m_MaxDepth);

	return closestPointFromLocation(location, p);

}

int Tree3d::closestPointFromLocation(Tree3d::Node* location, Point3d p)
{
	// location is not a leaf node if the value is bigger than the median, but no right child exitsts
	if (!location->isLeaf())
	{
		return closestPointFromLocation(location->getLeftChild(), p);
	}
	else
	{
		std::vector<int> ind_points = location->getIndices();

		Point3d t_point = indexToPoint(ind_points[0]);
		double t_dist = sqDistance3d(p, t_point);
		int index = ind_points[0];

		for (int i = 1; i < ind_points.size(); i++){
			t_point = indexToPoint(ind_points[i]);

			
			double t_t_dist = sqDistance3d(p, t_point);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				index = ind_points[i];
			}
		}

		t_dist = std::sqrt(t_dist);
		Point3d d = Point3d(t_dist, t_dist, t_dist);
		Point3d q1 = p + d;
		Point3d q2 = p - d;

		std::vector<int> ind_points_range = rangeQuery(q1, q2);

		t_point = indexToPoint(ind_points_range[0]);
		t_dist = sqDistance3d(p, t_point);
		index = ind_points_range[0];


		for (int i = 1; i < ind_points_range.size(); i++)
		{
			t_point = indexToPoint(ind_points_range[i]);
			float t_t_dist = sqDistance3d(p, t_point);
			if (t_t_dist < t_dist){
				t_dist = t_t_dist;
				index = ind_points_range[i];
			}
		}

		return index;
	}
}



