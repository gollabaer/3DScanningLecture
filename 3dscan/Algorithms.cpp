#include "Algorithms.h"
#include "SVD.h"

namespace algorithms
{
	/** @brief Computes and returns the center of the point cloud.
	@param points vector of points
	*/
	Point3d computeCenter(const std::vector<Point3d>& points)
	{
	  //compute the mean value (center) of the points cloud
	  Point3d mean(0, 0, 0);

	  const size_t N = points.size();
	  if (N < 1) return mean; //an empty point cloud gets (0,0,0) as the center

	  for (size_t i = 0; i < N; ++i)
	  {
		mean += points[i];
	  }
	  mean *= 1.0 / N;

	  return mean;
	}

	/** @brief computes the 3x3 Varianz matrix as the base for a Principal Component Analysis.
	@param points vector of points
	@param M      3x3 matrix
	*/
	void computeCovarianceMatrix3x3(const std::vector<Point3d>& points, Matrix& M)
	{
	  M.resize(3, 3);
	  const ptrdiff_t N(points.size());
	  if (N<1) return;

	  //compute the mean value (center) of the points cloud
	  Point3d mean = computeCenter(points);

	  //Compute the entries of the (symmetric) covariance matrix
	  double Mxx(0), Mxy(0), Mxz(0), Myy(0), Myz(0), Mzz(0);
	#pragma omp parallel for reduction(+: Mxx,Mxy,Mxz,Myy,Myz,Mzz) //omp reduction enables parallel sum up of values
	  for (ptrdiff_t i = 0; i<N; ++i)
	  {
		const Point3d& pt = points[i];

		//generate mean-free coorinates
		const double x1(pt.x - mean.x);
		const double y1(pt.y - mean.y);
		const double z1(pt.z - mean.z);

		//Sum up the entries for the covariance matrix
		Mxx += x1*x1; Mxy += x1*y1; Mxz += x1*z1;
		Myy += y1*y1; Myz += y1*z1;
		Mzz += z1*z1;
	  }

	  //setting the sums to the matrix (division by N just for numerical reason if we have very large sums)
	  M(0, 0) = Mxx / N; M(0, 1) = Mxy / N; M(0, 2) = Mxz / N;
	  M(1, 0) = M(0, 1); M(1, 1) = Myy / N; M(1, 2) = Myz / N;
	  M(2, 0) = M(0, 2); M(2, 1) = M(1, 2); M(2, 2) = Mzz / N;
	}

	void fitLine(const std::vector<Point3d>& points, Point3d& pointOnLine, Point3d& lineDirection, std::vector<Point3d>* boundaries)
	{
		pointOnLine = computeCenter(points);
		Matrix M;
		computeCovarianceMatrix3x3(points, M);
		SVD::computeSymmetricEigenvectors(M);

		lineDirection =  Point3d(M(0, 0), M(1, 0), M(2, 0)); //first column of M == Eigenvector corresponding to the largest Eigenvalue == direction of biggest variance

		if (boundaries != nullptr)
		{
			double minDist = 0;
			double maxDist = 0;

			for (auto it = points.begin(); it != points.end(); ++it)
			{
				double dist = distancePt2Plane(*it, pointOnLine, lineDirection);
				maxDist = (dist > maxDist) ? dist : maxDist;
				minDist = (dist < minDist) ? dist : minDist;
			}
			boundaries->push_back(pointOnLine + lineDirection * -1*minDist);
			boundaries->push_back(pointOnLine + lineDirection * -1*maxDist);
		}
	}

	void fitPlane(const std::vector<Point3d>& points, Point3d& pointOnPlane, Point3d& planeNormal, std::vector<Point3d>* boundaries)
	{
		pointOnPlane = computeCenter(points);
		Matrix M;
		computeCovarianceMatrix3x3(points, M);
		SVD::computeSymmetricEigenvectors(M);

		planeNormal = Point3d(M(0, 2), M(1, 2), M(2, 2)); //third column of M == Eigenvector corresponding to the smallest Eigenvalue == direction of lowest variance
		if (boundaries != nullptr)
		{
			// use the other eigenvectors as coordinate axis on the plane
			Point3d dir1 = Point3d(M(0, 0), M(1, 0), M(2, 0));
			Point3d dir2 = Point3d(M(0, 1), M(1, 1), M(2, 1));
			double maxDist1 = 0;
			double minDist1 = 0;
			double maxDist2 = 0;
			double minDist2 = 0;

			// calculate the signed distances of each point to the "helper-planes" orthogonal to the axis on the plane and find the points furthest away on both sides 
			for (auto it = points.begin(); it != points.end(); ++it)
			{
				double dist1 = distancePt2Plane(*it, pointOnPlane, dir1);
				double dist2 = distancePt2Plane(*it, pointOnPlane, dir2);
				
				maxDist1 = (dist1 > maxDist1) ? dist1 : maxDist1;
				maxDist2 = (dist2 > maxDist2) ? dist2 : maxDist2;
				minDist1 = (dist1 < minDist1) ? dist1 : minDist1;
				minDist2 = (dist2 < minDist2) ? dist2 : minDist2;
			}

			// calculate the corner points based on the furthest distances calculated in the previous step
			boundaries->push_back(pointOnPlane + (dir1 * -1 * minDist1) + (dir2 * -1 * minDist2)); //a
			boundaries->push_back(pointOnPlane + (dir1 * -1 * minDist1) + (dir2 * -1 * maxDist2)); //b
			boundaries->push_back(pointOnPlane + (dir1 * -1 * maxDist1) + (dir2 * -1 * maxDist2)); //c
			boundaries->push_back(pointOnPlane + (dir1 * -1 * maxDist1) + (dir2 * -1 * minDist2)); //d
		}
	}

	/** @brief Computes the distance of a point to a 3D line.
		@param point         point 
		@param pointOnLine   a point on the line (e.g. "center" point)
		@param lineDirection vector respresenting the 3d direction of the line (must be a unit vector -> length==1)
	*/
	double distancePt2Line(const Point3d& point, const Point3d& pointOnLine, const Point3d& lineDirection)
	{
	  const Point3d PQ = pointOnLine - point;
	  double distance = vectorLength( crossProduct(PQ, lineDirection) );

	  return distance;
	}

	/** @brief Computes the distance of a point to a plane.
	@param point          point
	@param pointOnPlane   a point on the plane (e.g. "center" point)
	@param planeDirection vector respresenting the 3d direction of the plane (must be a unit vector -> length==1)
	*/
	double distancePt2Plane(const Point3d& point, const Point3d& pointOnPlane, const Point3d& planeDirection )
	{
	  const Point3d PQ = pointOnPlane - point;
	  double distance = dotProduct(PQ, planeDirection);

	  return distance;
	}
}