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
	/**
	Constructs a 3dimensional KD-Tree, containing the given points.
	
	Construction stops at given Tree depth where all points are added to the leaf.
	*/
	Tree3d(std::vector<Point3d> &points, int maxDepth); // makes a copy of points
	~Tree3d(); ///<Desctructor Tree3D
	// Getter
	int getNumberOfPoints(); ///< returns the number of points stored in the kd Tree
	std::vector<Point3d> getPoints(); ///< returns a vector containing all stored points
	// Functions
	/**
	Axis aligned  range query in the KD-Tree

	Returns the indices of the points contained in the given range,	specified by 2 points spanning am axis aligned point.
	*/
	std::vector<int> rangeQuery(Point3d p1, Point3d p2); ///< returns the indices of all points inside the box formed by p1 and p2
	/**
	Sphereical query in KD-tree

	Returns the indices of the points contained in the sphere,	specified by the center and radius.
	*/
	std::vector<int> radiusQuery(Point3d queryPoint, double radius); ///< returns the indices of all points inside the sphere at middle point with radius
	/**
	Nearest Neighbour query for given Point

	Returns the index of the Point closest to the query point.
	*/
	int nearestNeighbour(Point3d p); ///< index to the nearest neighbour
	/**
	Smoothes the pointcloud in the KD-Tree

	Returns a vector containing the smoothed points, which are smoothed by averaging the coordinates over the points in given radius.
	*/
	std::vector<Point3d> applySmoothing(double radius); ///< generates a smoothed pointclound by averaging the local neighbourhood
	/**
	Calulates the distance per point to pointcloud

	Distance of a point to the point cloud is given by the distance
	to it's nearest neighbour in the pointcloud.
	*/
	std::vector<double> calculateDistance(std::vector<Point3d> other); ///< calculate for each point in other the distance to the closest point in this tree
	/**
	Thins the point cloud

	Marks points as removed such that the pairwise distances are larger than given distance and returns a vector containing the not removed points
	*/
	std::vector<Point3d> applyThinningByRadius(double r); ///< thins the point cloud, so that only one point remains in the given radius
	std::vector<Point3d> getThinnedPoints(); ///< returns all points in the tree not marked as removed
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
		void rangeQuery(std::vector<int> &outIndices, Point3d &lowerBoundary, Point3d &upperBoundary, std::vector<Point3d> &points); ///< recursively reports points in the subtree of the node inbetween the lower and upper boundary
		bool inline pointIsInRange(int index, Point3d &lowerBoundary, Point3d &upperBoundary, std::vector<Point3d> &points); ///< test if the point defined by index is located inbetween the lower and upper boundary in every DIMension
		void nearestNeighbour(int &index, double &currentMinimumDistance, Point3d queryPoint, std::vector<Point3d> &points); ///< recursivly search for the nearest neighbor of the queryPoint
		void radiusQuery(std::vector<int> &outIndices, Point3d &queryPoint, double &radius, std::vector<Point3d> &points); ///< recursively reports points in the subtree of the node inside the defined sphere
		void thinning(Point3d &point, std::vector<bool> &flags, double radius, std::vector<Point3d> &points); ///< marks points inside the radius query as removed
		bool isRemoved();
		
	private:
		// Member Variables
		Node* m_Parent; ///< Parent Node
		Node* m_LeftChild; ///< Child Node <= Median
		Node* m_RightChild; ///< Child Node > Median
		double m_Median; ///< Median value that was used for splitting on current axis
		double m_Min; ///< Minimum Value of Node's points on current axis
		double m_Max; ///< Maximum Value of Node's points on current axis
		int m_Axis; ///< x = 0, y = 1, z = 2
		std::vector<int>* m_Indices; ///< Indices of Node's points
		bool m_removedFlag; ///< shows if point has been removed
	};
	// Member Variables
	Node* m_Root; ///< root node of the tree
	std::vector<Point3d> m_Points; ///< list of all the point stored in the tree
	std::vector<bool> m_pointremovedFlags; ///< corresponding  to list of points
	int m_MaxDepth; ///< maximum depth of the tree
	// Functions
};

