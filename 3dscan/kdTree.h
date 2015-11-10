#include <vector>
#include <algorithm>  
#include <QVector3D>


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
	std::vector<int> rangeQuery(QVector3D p1, QVector3D p2); // returns the indices of all points inside the box formed by p1 and p2
	std::vector<int> nearestNeighbor(QVector3D p); // the coordinates of the nearest neighbor
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
		std::vector<int> reportPoints(int depth, std::vector<float> &points, std::vector<float> &a, std::vector<float> &b, int &dim);
		bool testPointInRange(int index, int axis, std::vector<float> &points, std::vector<float> &a, std::vector<float> &b, int &dim);
		Node* locatePoint(QVector3D p, int depth, int &dim);
	private:
		// Member Variables
		Node* m_Parent; // Parent Node
		Node* m_LeftChild; // Child Node <= Median
		Node* m_RightChild; // Child Node > Median
		int m_Median; // Median value that was used for splitting on current axis
		float m_Min; // Minimum Value of Node's points on current axis
		float m_Max; // Maximum Value of Node's points on current axis
		std::vector<int>* m_Indices; // Indices of Node's points
	};
	// Member Variables
	Node* m_Root;
	std::vector<float> m_Points;
	int m_MaxDepth;
	int m_Dim;
	// Functions
};

