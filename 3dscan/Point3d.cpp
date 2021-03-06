#include <cmath>        //for standard C/C++ math functions
#include "Point3d.h" 

//custom operator that enables the + operation of two points (pt3 = pt1 + pt2)
Point3d Point3d::operator + (const Point3d& p2) const
{
  Point3d result;
  result.x = x + p2.x;
  result.y = y + p2.y;
  result.z = z + p2.z;
  return result;
}

//custom operator that enables the - operation of two points (pt3 = pt1 - pt2)
Point3d Point3d::operator - (const Point3d& p2) const
{
  Point3d result;
  result.x = x - p2.x;
  result.y = y - p2.y;
  result.z = z - p2.z;
  return result;
}
//custom operator that enables the multiplication with a scalar value (pt2 = pt1 * 0.5)
Point3d Point3d::operator * (double scalar) const
{
  Point3d result;
  result.x = scalar * x;
  result.y = scalar * y;
  result.z = scalar * z;
  return result;
}

// acces element by index, 0,1,2
const double &Point3d::operator [] (int index) const
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		throw std::out_of_range("Index of Point3d has to be in [0 .. 2]");
	}
}

// acces element by index, 0,1,2
double &Point3d::operator [] (int index)
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		throw std::out_of_range("Index of Point3d has to be in [0 .. 2]");
	}
}

//custom operator that enables the += operation (pt1 += pt2 -> pt1 = pt1 + pt2)
Point3d& Point3d::operator += (const Point3d& p2)
{
  x += p2.x;
  y += p2.y;
  z += p2.z;
  return *this;
}

//custom operator that enables the -= operation (pt1 -= pt2 -> pt1 = pt1 - pt2)
Point3d& Point3d::operator -= (const Point3d& p2)
{
  x -= p2.x;
  y -= p2.y;
  z -= p2.z;
  return *this;
}

//custom operator that enables the += operation (pt1 *= 2 -> pt1 = pt1 * s)
Point3d& Point3d::operator *= (double scalar)
{
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

Point3d& Point3d::operator /= (double scalar)
{
	// TODO: find better epsilon and better place to define it
	const double EPSILON = 0.00000000001;
	if (scalar - 0 < EPSILON)
	{
		throw std::invalid_argument("division by Zero");
	}
	return this->operator*=(1 / scalar);
}

bool Point3d::operator == (const Point3d& p2) const
{
	
	return ((this->x == p2.x) && (this->y == p2.y) && (this->y == p2.y)) ;
}


//returns the square of a value (unfortunately C++ does not provide this function itself...)
double sqr(double value)
{
  return value*value;
}

//returns the length of a vector
double  vectorLength(const Point3d& v)
{
  double length = sqrt(sqr(v.x) + sqr(v.y) + sqr(v.z));
  return length;
}

//returns the dot product of two 3d vectors
double dotProduct(const Point3d& v1, const Point3d& v2)
{
  return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
}

//returns the cross product of two 3d vectors
Point3d crossProduct(const Point3d& v1, const Point3d& v2)
{
  Point3d result;
  result.x = (v1.y * v2.z) - (v1.z * v2.y);
  result.y = (v1.z * v2.x) - (v1.x * v2.z);
  result.z = (v1.x * v2.y) - (v1.y * v2.x);

  return result;
}

//normalizes a 3d vector (direction vector)
void normalizeVector(Point3d& v)
{
  const double length = vectorLength(v);
  if (length > 0)
  {
    v.x /= length;
    v.y /= length;
    v.z /= length;
  }
}

///< returns the squared Euclidean distance between two 3d points/vectors
double sqDistance3d(const Point3d& v1, const Point3d& v2)
{
  const double d = sqr(v1.x - v2.x) + sqr(v1.y - v2.y) + sqr(v1.z - v2.z);
  return d;
}

//returns the Euclidean distance between two 3d points / vectors
double distance3d(const Point3d& v1, const Point3d& v2)
{
  const double d = std::sqrt(sqDistance3d(v1,v2));
  return d;
}