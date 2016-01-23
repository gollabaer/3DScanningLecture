#include "Tree3d.h"
#include <assert.h> 
#include <limits>  // std::numeric_limits
#include "Algorithms.h"
#include "SVD.h"

const int DIM = 3;

/*-------------------------------------------------------------*/
// Constructors
/*-------------------------------------------------------------*/

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
	m_pointremovedFlags = std::vector<bool>();
	m_pointremovedFlags.resize(points.size());
	for (int i = 0; i < points.size(); i++) m_pointremovedFlags[i] = 0;
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

/*-------------------------------------------------------------*/
// Getters
/*-------------------------------------------------------------*/

int Tree3d::getNumberOfPoints()
{
	return m_Points.size();
}

std::vector<Point3d> Tree3d::getPoints()
{
	return m_Points;
}

/*-------------------------------------------------------------*/
// Functions
/*-------------------------------------------------------------*/

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
	std::vector<int> result; result.clear();
	this->m_Root->rangeQuery(result, lowerBoundary, upperBoundary, this->m_Points);
	return result;
}

std::vector<int> Tree3d::radiusQuery(Point3d queryPoint, double radius)
{
	std::vector<int> outIndices;

	this->m_Root->radiusQuery(outIndices, queryPoint, radius, this->m_Points);
	return outIndices;
}

int Tree3d::nearestNeighbour(Point3d p)
{
	// intialize index
	int index = -1;

	// use maximum double value for initialization
	double dist = std::numeric_limits<double>::max();

	m_Root->nearestNeighbour(index, dist, p, m_Points);
	return index;
}

std::vector<Point3d> Tree3d::applySmoothing(double radius)
{
	std::vector<Point3d> smoothedCloud;
	Point3d average = Point3d(0, 0, 0);

	// for all points compute new position as average of neighbours
	for (auto it = m_Points.begin(); it != m_Points.end(); ++it)
	{
		std::vector<int> neighbourIndices = radiusQuery(*it, radius);
		average.x = 0;
		average.y = 0;
		average.z = 0;
		// extract averaging as own function
		for (auto indexIt = neighbourIndices.begin(); indexIt != neighbourIndices.end(); ++indexIt)
		{
			average += m_Points[*indexIt];
		}
		average /= neighbourIndices.size();
		smoothedCloud.push_back(average);
	}
	return smoothedCloud;
}

std::vector<double> Tree3d::calculateDistance(std::vector<Point3d> other)
{
	std::vector<double> distances;
	// for each point in other compute nearest neighbour distance
	for (auto it = other.begin(); it != other.end(); ++it)
	{
		Point3d nearesNeighbour = m_Points[nearestNeighbour(*it)];
		distances.push_back(distance3d(*it, nearesNeighbour));
	}
	return distances;
}

std::vector<Point3d> Tree3d::getThinnedPoints()
{
	std::vector<Point3d> thinnedPoints;

	for (int i = 0; i < m_Points.size(); i++){
		if (!m_pointremovedFlags[i])
			thinnedPoints.push_back(m_Points[i]);
	}

	return thinnedPoints;
}

std::vector<Point3d> Tree3d::applyThinningByRadius(double r)
{
	for (int i = 0; i < m_Points.size(); i++){
		if (!m_pointremovedFlags[i]){
			m_Root->thinning(m_Points[i], m_pointremovedFlags, r, m_Points);
		}
	}
	return getThinnedPoints();
}

/*-------------------------------------------------------------*/
// Node Class
/*-------------------------------------------------------------*/

/*----------------------------*/
//Constructors
/*----------------------------*/

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
	m_removedFlag = false;

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

void Tree3d::computeNormals(double radius, std::vector<Point3d> &normals){

	normals.resize(m_Points.size());

	for (size_t i = 0; i < m_Points.size(); i++){

		std::vector<int> neighboursIndices = this->radiusQuery(m_Points[i], radius);
		std::vector<Point3d> neighbourPoints;

		for (int j = 0; j < neighboursIndices.size(); j++)
			neighbourPoints.push_back(this->m_Points[neighboursIndices[j]]);


		if (neighbourPoints.size() > 2){
			Matrix M(3, 3);
			algorithms::computeCovarianceMatrix3x3(neighbourPoints, M);
				
				SVD::computeSymmetricEigenvectors(M);
				Point3d normal = Point3d(M(0, 2), M(1, 2), M(2, 2));
				normals[i] = normal;
			}
			else
				normals[i] = Point3d(0, 0, 0);
	}
}


/*----------------------------*/
//Getters
/*----------------------------*/

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

double Tree3d::Node::getMax()
{
	return m_Max;
}

double Tree3d::Node::getMin()
{
	return m_Min;
}

bool Tree3d::Node::isRemoved()
{ 
	return this->m_removedFlag; 
}

std::vector<int> Tree3d::Node::getIndices()
{
	if (m_Indices != NULL)
		return *m_Indices;
	else
		return std::vector<int> ();

}

/*----------------------------*/
//Functions
/*----------------------------*/

bool Tree3d::Node::isLeaf()
{
	if (this->m_Indices != NULL) return true;
	else return false;
}

void Tree3d::Node::rangeQuery(std::vector<int> &outIndices, Point3d &lowerBoundary, Point3d &upperBoundary, std::vector<Point3d> &points)
{
	// if range query is out of bounds, return empty vector
	if (lowerBoundary[m_Axis] > this->m_Max || upperBoundary[m_Axis] < this->m_Min)
	{
		return;
	}

	// range queries a,b with min <= a <= median <b <= max

	// check right child and add points to result
	if (this->m_RightChild != NULL)
	{
		this->m_RightChild->rangeQuery(outIndices, lowerBoundary, upperBoundary, points);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL)
	{
		this->m_LeftChild->rangeQuery(outIndices, lowerBoundary, upperBoundary, points);
	}
	// if the left child is NULL the node has to be a leaf, so traverse points and return fitting ones
	else 
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (std::vector<int>::iterator it = this->m_Indices->begin(); it != this->m_Indices->end(); ++it)
		{
			// if point fits bounding box in every DIMension
			if (pointIsInRange(*it, lowerBoundary, upperBoundary, points))
			{
				outIndices.push_back(*it);
			}
		}
	}
}

bool Tree3d::Node::pointIsInRange(int index, Point3d &lowerBoundary, Point3d &upperBoundary, std::vector<Point3d> &points)
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

void Tree3d::Node::radiusQuery(std::vector<int> &outIndices, Point3d &queryPoint, double &radius, std::vector<Point3d> &points)
{
	// if the left child is NULL the node has to be a leaf, so traverse points and return fitting ones
	if (isLeaf())
	{
		// traverse (sorted) indices from front to back until value is bigger than median
		for (int i = 0; i < m_Indices->size(); i++)
		{

			// if point is in radius distance from query point
			if (sqDistance3d(points[m_Indices->operator[](i)], queryPoint) <= radius*radius)
			{
				outIndices.push_back(m_Indices->operator[](i));
			}
		}
	}	
	// check right child and add points to result
	if (this->m_RightChild != NULL 
		&& queryPoint[m_Axis] + radius >= m_Median)
	{
		this->m_RightChild->radiusQuery(outIndices, queryPoint, radius, points);

	}
	// check left child and add points to result
	if (this->m_LeftChild != NULL 
		&& queryPoint[m_Axis] - radius <=  m_Median)
	{
		this->m_LeftChild->radiusQuery(outIndices, queryPoint, radius, points);

	}
	
}

void Tree3d::Node::nearestNeighbour(int &index, double &currentMinimumDistance, Point3d queryPoint, std::vector<Point3d> &points)
{

	// get position of quey point on current axis
	double queryPointAxisValue = queryPoint[m_Axis];

	//report nearest neighbour in leaf
	if (isLeaf()) {

		double currentDistance =  sqDistance3d(queryPoint, points[m_Indices->operator[](0)]);
		int currentIndex = m_Indices->operator[](0);

		//find closest point in node 
		for (int i = 1; i < m_Indices->size(); i++)
		{
			double newDistance = sqDistance3d(queryPoint, points[m_Indices->operator[](i)]);
			// if newDistance is smaller than currentDistance save newDistance
			if (newDistance < currentDistance){
				currentDistance = newDistance;
				currentIndex = m_Indices->operator[](i);
			}
		}
		// if node's smallest distance is smaller than currentMinimumDistance save currentDistance
		if (currentMinimumDistance > currentDistance){
			currentMinimumDistance = currentDistance;
			index = currentIndex;
			return;
		}
	}
	// if node is not a leaf and smaller/equal median -> traverse left child
	if (queryPointAxisValue <= m_Median)
	{
		if (m_LeftChild != NULL)
			m_LeftChild->nearestNeighbour(index, currentMinimumDistance, queryPoint, points);

		if (queryPointAxisValue + currentMinimumDistance > m_Median && m_RightChild != NULL)
			m_RightChild->nearestNeighbour(index, currentMinimumDistance, queryPoint, points);
	}
	// if node is not a leaf and bigger than median -> traverse right child
	else
	{
		if (m_RightChild != NULL)
			m_RightChild->nearestNeighbour(index, currentMinimumDistance, queryPoint, points);

		if (queryPointAxisValue - currentMinimumDistance < m_Median && m_LeftChild != NULL)
			m_LeftChild->nearestNeighbour(index, currentMinimumDistance, queryPoint, points);
	}
}

void Tree3d::Node::thinning(Point3d &point, std::vector<bool> &pointIsRemovedFlags, double radius, std::vector<Point3d> &points)
{
	//if leave check indices
	if (this->isLeaf())
	{
		bool isremoved = true;
		for (int i = 0; i < m_Indices->size(); i++)
		{
			int index = m_Indices->operator[](i);
			//if point still "alive"
			if (!pointIsRemovedFlags[index]){
				//if point in radius
				if ((sqDistance3d(point, points[index])) <= sqr(radius)
					&& !(point == points[index]))
				{
					pointIsRemovedFlags[index] = true;
				}
				else
				{
					isremoved = false;
				}
			}
		}
		m_removedFlag = isremoved;
	}
	//if not leave recurse
	else{
		
		if (m_LeftChild != 0 && !m_LeftChild->isRemoved() 
			&& point[m_Axis]-radius <= m_Median ){
			m_LeftChild->thinning(point, pointIsRemovedFlags, radius, points);
		}
		if (m_RightChild != 0 && !m_RightChild->isRemoved()
			&& point[m_Axis]+radius >= m_Median){
			m_RightChild->thinning(point, pointIsRemovedFlags, radius, points);
			
		}
		if ((m_LeftChild == 0 || m_LeftChild->isRemoved()) && (m_RightChild == 0 || m_RightChild->isRemoved()))
			m_removedFlag = true;
	}
}


