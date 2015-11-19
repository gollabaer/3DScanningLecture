#include <vector>
#include <algorithm>  
#include <QVector3D>
#include "Point3d.h"


#pragma once
class Tree3d
{
public:
	// Constructors
	Tree3d();
	Tree3d(std::vector<Point3d> points, int maxDepth);
	~Tree3d();
	// Getter
	int getNumberOfPoints(); // returns the number of points stored in the kd Tree
	std::vector<Point3d> getPoints(); // returns a vector containing all stored points
	// Functions
	std::vector<int> rangeQuery(Point3d p1, Point3d p2); // returns the indices of all points inside the box formed by p1 and p2
	int nearestNeighbor(Point3d p); // the coordinates of the nearest neighbor
private:
	// Node Class
	class Node
	{
	public:
		// Constructors
		Node();
		~Node();
		Node(std::vector<int>* indices, std::vector<Point3d> &points, int depth, Node* parent);
		// Getter
		Node* getParent();
		Node* getLeftChild();
		Node* getRightChild();
		int getMedian();
		float getMax();
		float getMin();
		std::vector<int> getIndices();
		bool isLeaf();
		std::vector<int> reportPoints(int depth, std::vector<Point3d> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary); // recursively reports points in the subree of the node inbetween the lower and upper boundary
		bool testPointInRange(int index, int axis, std::vector<Point3d> &points, std::vector<float> &lowerBoundary, std::vector<float> &upperBoundary); // test if the point defined by index is located inbetween the lower and upper boundary in every DIMension
		Node* locatePoint(Point3d p, int depth); // locate the leaf node which would contain p
	private:
		// Member Variables
		Node* m_Parent; // Parent Node
		Node* m_LeftChild; // Child Node <= Median
		Node* m_RightChild; // Child Node > Median
		float m_Median; // Median value that was used for splitting on current axis
		float m_Min; // Minimum Value of Node's points on current axis
		float m_Max; // Maximum Value of Node's points on current axis
		std::vector<int>* m_Indices; // Indices of Node's points
	};
	// Member Variables
	Node* m_Root;
	std::vector<Point3d> m_Points;
	int m_MaxDepth;
	// Functions
	Point3d indexToPoint(int i);
	int Tree3d::closestPointFromLocation(Tree3d::Node* location, Point3d p);
};

