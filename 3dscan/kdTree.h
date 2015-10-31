#include <vector>
#include <algorithm>  
#include <QDebug>

#pragma once
class kdTree
{
public:
	// Constructors
	kdTree();
	kdTree(std::vector<float> points, int maxDepth, int dim);
	~kdTree();
	// Getter
	int getNumberOfPoints();
	std::vector<float> getPoints();
private:
	// Node Class
	class Node
	{
	public:
		// Constructors
		Node();
		~Node();
		Node(std::vector<int> indices, std::vector<float> &points, int depth, int dim, Node* parent);
	private:
		// Member Variables
		Node* m_Parent; // Parent Node
		Node* m_LeftChild; // Child Node <= Median
		Node* m_RightChild; // Child Node > Median
		int m_Median; // Median value that was used for splitting on current axis
		float m_Min; // Minimum Value of Node's points on current axis
		float m_Max; // Maximum Value of Node's points on current axis
		std::vector<int> m_Indices; // Indices of Node's points
	};
	// Member Variables
	Node* m_Root;
	std::vector<float> m_Points;
	int m_MaxDepth;
	int m_Dim;
};

