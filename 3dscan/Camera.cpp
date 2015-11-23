#include "Camera.h"


Camera::Camera(void) :
			vertical_fov(INIT_VFOV)
	{
	}


Camera::~Camera(void)
	{
	}

const float Camera::INIT_VFOV = 60.0f;
const float Camera::MIN_VFOV = 3.0f;
const float Camera::MAX_VFOV = 180.0f;

std::vector<float> Camera::init(std::vector<Point3d> &points, float width, float height){
		float xmin = points[0].x, xmax = points[0].x;
		float ymin = points[0].y, ymax = points[0].y;
		float zmin = points[0].z, zmax = points[0].z;

		for (int i = 0; i < points.size(); i ++){
			// x-dim
			xmin = (points[i].x < xmin) ? points[i].x : xmin;
			xmax = (points[i].x > xmax) ? points[i].x : xmax;
			// y-dim
			ymin = (points[i].y < ymin) ? points[i].y : ymin;
			ymax = (points[i].y > ymax) ? points[i].y : ymax;
			// z-dim
			zmin = (points[i].z < zmin) ? points[i].z : zmin;
			zmax = (points[i].z > zmax) ? points[i].z : zmax;
		}

		float dist = 2 * (zmax - zmin);

		std::vector<float> box;
		box.push_back(xmin); box.push_back(ymin); box.push_back(zmin);
		box.push_back(xmin); box.push_back(ymin); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymin); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymin); box.push_back(zmin);

		box.push_back(xmin); box.push_back(ymax); box.push_back(zmin);
		box.push_back(xmin); box.push_back(ymax); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymax); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymax); box.push_back(zmin);

		init((xmax - xmin) / 2.0f + xmin, (ymax - ymin) / 2.0f +ymin, (zmax - zmin) / 2.0f + zmin, dist, width, height);

		zoomfactor = dist / 200.0f;

		return box;
	}
