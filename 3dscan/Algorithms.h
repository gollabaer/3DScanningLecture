#ifndef MY_ALGORITHMS_H
#define MY_ALGORITHMS_H

#include "Point3d.h"
#include "Matrix.h"
#include <vector>

namespace algorithms
{
	Point3d computeCenter(const std::vector<Point3d>& points);                  ///< Computes and returns the center of the point cloud
	void computeCoarianceMatrix(const std::vector<Point3d>& points, Matrix& M); ///< Coputes the 3x3 covariance matrix

	/** @brief Computes the best fitting line using the eigenvector with the largest value of the CovarianceMatrix.
	@param points        pointcloud to fit plane into
	@param pointOnLine   a point on the fitted line (e.g. "center" point)
	@param lineDirection vector respresenting the 3d direction of the fitted line
	*/
	void fitLine(const std::vector<Point3d>& points, Point3d& pointOnLine, Point3d& lineDirection, std::vector<Point3d>* boundaries = nullptr);

	/** @brief Computes the best fitting line using the eigenvector with the smallest value of the CovarianceMatrix.
	@param points       pointcloud to fit plane into
	@param pointOnPlane a point on the fitted plane (e.g. "center" point)
	@param planeNormal	vector respresenting the 3d normal of the fitted plane
	*/
	void fitPlane(const std::vector<Point3d>& points, Point3d& pointOnPlane, Point3d& planeNormal, std::vector<Point3d>* boundaries = nullptr);

	/** @brief Computes the best fitting sphere using nonlinear least squares.
	@param points       pointcloud to fit sphere into
	@param [out] center the center of the fitted sphere
	@param [out] radius	the radius of the fitted sphere
	*/
	void fitSphere(const std::vector<Point3d>& points, Point3d& center, double& radius);

	double distancePt2Line(const Point3d& point, const Point3d& pointOnLine, const Point3d& lineDirection);  ///< distance point-to-line (3d)
	double distancePt2Plane(const Point3d& point, const Point3d& pointOnPlane, const Point3d& planeDirection);  ///< distance point-to-plane

}
#endif //MY_ALGORITHMS_H
