#include <vector>
#include <algorithm>  
#include <QVector3D>

const int TREE_DIM = 3;

#pragma once
class kdTree
{
public:
	// Constructors
	kdTree();
	kdTree(std::vector<float> points, int maxDepth, int dim);
	~kdTree();
	// Getter
	int getNumberOfPoints(); // returns the number of points stored in the kd Tree
	std::vector<float> getPoints(); // returns a vector containing all stored points
	// Functions
	std::vector<int> rangeQuery(std::vector<float> p1, std::vector<float> p2); // returns the indices of all points inside the box formed by p1 and p2
	std::vector<int> radiusQuery(std::vector<float> queryPoint, float radius);
	int nearestNeighbor(std::vector<float> p); // the coordinates of the nearest neighbor
	static double squaredEuclidianDistance(std::vector<float> &p1, std::vector<float> &p2);
	static inline std::vector<float> indexToVector(int i, std::vector<float> &points);
private:
	// Node Class
	class Node
	{
	public:
		// Constructors
		Node();
		~Node();
		Node(std::vector<int>* indices, std::vector<float> &points, int depth, int dim, Node* parent);
		// Getter
		Node* getParent();
		Node* getLeftChild();
		Node* getRightChild();
		int getMedian();
		float getMax();
		float getMin();
		std::vector<int> getIndices();
		bool isLeaf();
		std::vector<int> reportPoints(int depth, std::vector<float> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary, int &dim); // recursively reports points in the subree of the node inbetween the lower and upper boundary
		bool testPointInRange(int index, int axis, std::vector<float> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary, int &dim); // test if the point defined by index is located inbetween the lower and upper boundary in every dimension
		Node* locatePoint(std::vector<float> p, int depth, int &dim); // locate the leaf node which would contain p
		void NearesNeighbour(std::vector<float> queryPoint, float &currentRange, int &index,std::vector<float> &points);
	private:
		// Member Variables
		Node* m_Parent; // Parent Node
		Node* m_LeftChild; // Child Node <= Median
		Node* m_RightChild; // Child Node > Median
		float m_Median; // Median value that was used for splitting on current axis
		float m_Min; // Minimum Value of Node's points on current axis
		float m_Max; // Maximum Value of Node's points on current axis
		int m_axis;
		std::vector<int>* m_Indices; // Indices of Node's points
	};
	// Member Variables
	Node* m_Root;
	std::vector<float> m_Points;
	int m_MaxDepth;
	int m_Dim;
	// Functions
	int kdTree::closestPointFromLocation(kdTree::Node* location, std::vector<float> p);
};

