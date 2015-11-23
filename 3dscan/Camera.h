#pragma once
#include <vector>

#include <QMatrix4x4>
#include <qmath.h>

extern "C" {
#include "trackball.h"
}
#include "Point3d.h"
class Camera{

public:

	Camera();
	~Camera();

	// sets up the camera to look at the center of points
	std::vector<float> init(std::vector<Point3d> &points, float width, float height);

	// sets up a camera looking at (x,y,z) from the distance d and with an aspectratio of w/h 
	void init(float x, float y, float z, float d, float w, float h){
		center = QVector3D(x, y, z);
		dir = QVector3D(0, 0, d);
		qrotation = QQuaternion();
		distance = d;

		update();
	}

	void rotate(QQuaternion q){
		qrotation = q * qrotation;
		update();
	}

	void rotate(float x, float y, float xO, float yO){
		// define center of window
		float centerX = float(width / 2);
		float centerY = float(height / 2);

		//difference of new and old mouse position
		float oldX = (xO - centerX) / centerX;
		float oldY = (yO - centerY) / centerY;
		float newX = (x - centerX) / centerX;
		float newY = (y - centerY) / centerY;

		//model.rotate(2, -xDiff, 0, -yDiff);
		float rotation[4];
		// get rotation as quaterion from trackball function
		gfs_gl_trackball(rotation, newX, newY, oldX, oldY);
		QQuaternion q = QQuaternion(QVector4D(rotation[0], rotation[1], rotation[2], rotation[3]));
		rotate(q);
	}

	void zoom(float dX){
		distance = ((distance + dX) > 1.0f) ? (distance + dX) : 1.0f;
		update();
	}

	void resize(int w, int h){
		width = w;
		height = h;
		update();
	}

	float getVFov() const {
		return this->vertical_fov;
	}
	void setVFov(float fov){
		this->vertical_fov = clip(fov, MIN_VFOV, MAX_VFOV);
		update();
	}
	void incVFov(float inc){
		this->vertical_fov = clip(vertical_fov + inc, MIN_VFOV, MAX_VFOV);		
		update();
	}

	QMatrix4x4 getCombinedMatrix() const {
		return projection * view * model;
	}

	QMatrix4x4 getModelMatrix() const {
		return model;
	}

	QMatrix4x4 getViewMatrix() const {
		return view;
	}

	QMatrix4x4 getProjMatrix() const {
		return projection;
	}

private:
	static const float INIT_VFOV;
	static const float MIN_VFOV;
	static const float MAX_VFOV;

	QMatrix4x4 model, view, projection;
	QQuaternion qrotation;
	QVector3D center, dir;
	float distance;
	float width, height;
	float zoomfactor;
	float vertical_fov;

	// TODO: put (as tmplate) in general helper class/namespace
	float clip(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}
	
	void update(){
		// apply rotation to scene
		QMatrix4x4 m1;
		m1.rotate(qrotation);

		model.setToIdentity();
		model.translate(center);
		model = model * m1;
		model.translate(-center);

		// apply projection
		projection.setToIdentity();
		projection.perspective(vertical_fov, width / height, 0.1f, 1000.0f);

		// setup viewing direction
		view.setToIdentity();
		view.lookAt(center - dir, center, QVector3D(0, 1, 0));
	}
	
};

