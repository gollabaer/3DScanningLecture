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
	Tree3d(std::vector<Point3d> &points, int maxDepth);
	~Tree3d();
	// Getter
	int getNumberOfPoints(); // returns the number of points stored in the kd Tree
	std::vector<Point3d> getPoints(); // returns a vector containing all stored points
	// Functions
	std::vector<int> rangeQuery(Point3d p1, Point3d p2); // returns the indices of all points inside the box formed by p1 and p2
	std::vector<int> radiusQuery(Point3d queryPoint, double radius); // returns the indices of all points inside the sphere at middle point with radius
	int nearestNeighbour(Point3d p); // index to the nearest neighbour
	std::vector<Point3d> applySmoothing(double radius); // generates a smoothed pointclound by averaging the local neighbourhood
	std::vector<double> calculateDistance(std::vector<Point3d> other); // calculate for each point in other the distance to the closest point in this tree
private:
	// Node Class
	class Node
	{
	public:
		// Constructors
		Node();
		~Node();
		Node(std::vector<int>* indices, std::vector<Point3d> &points, int depth, Node* parent);
		// Getters
		Node* getParent();
		Node* getLeftChild();
		Node* getRightChild();
		int getMedian();
		double getMax();
		double getMin();
		std::vector<int> getIndices();
		bool isLeaf();
		// Functions
		std::vector<int> rangeQuery(std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary); // recursively reports points in the subtree of the node inbetween the lower and upper boundary
		bool inline pointIsInRange(int index, std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary); // test if the point defined by index is located inbetween the lower and upper boundary in every DIMension
		void nearestNeighbour(Point3d queryPoint, double &currentMinimumDistance, int &index, std::vector<Point3d> &points); 
		std::vector<int> radiusQuery(std::vector<Point3d> &points, Point3d &lowerBoundary, Point3d &upperBoundary, Point3d &queryPoint, double &radius);// recursively reports points in the subtree of the node inside the defined sphere
	private:
		// Member Variables
		Node* m_Parent; // Parent Node
		Node* m_LeftChild; // Child Node <= Median
		Node* m_RightChild; // Child Node > Median
		double m_Median; // Median value that was used for splitting on current axis
		double m_Min; // Minimum Value of Node's points on current axis
		double m_Max; // Maximum Value of Node's points on current axis
		int m_Axis; // x = 0, y = 1, z = 2
		std::vector<int>* m_Indices; // Indices of Node's points
	};
	// Member Variables
	Node* m_Root;
	std::vector<Point3d> m_Points;
	int m_MaxDepth;
	// Functions
};

