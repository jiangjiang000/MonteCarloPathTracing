#include "BoundingBox.h"
#include <algorithm>

#include "utlis.h"

using namespace std;

BoundingBox::BoundingBox()
{

}

BoundingBox::BoundingBox(Vec3f min_b, Vec3f max_b)
{
	min_bounding = min_b;
	max_bounding = max_b;
}

BoundingBox::~BoundingBox()
{

}

//合并2个AABB
BoundingBox BoundingBox::expandbox(BoundingBox box_)
{
	//取两个包围盒的最大值和最小值，组成新的包围盒
	Vec3f mi, ma;
	mi.x = min(min_bounding.x, box_.min_bounding.x);
	mi.y = min(min_bounding.y, box_.min_bounding.y);
	mi.z = min(min_bounding.z, box_.min_bounding.z);
	ma.x = max(max_bounding.x, box_.max_bounding.x);
	ma.y = max(max_bounding.y, box_.max_bounding.y);
	ma.z = max(max_bounding.z, box_.max_bounding.z);
	min_bounding = mi;
	max_bounding = ma;
	return BoundingBox(mi, ma);
}

//计算ray与AABB交点
bool BoundingBox::boundingbox_intersect(Ray& ray, Intersection& result)
{
	Vec3f inter_point;
	float tx_min,ty_min,tz_min,tx_max,ty_max,tz_max;
	
	if (abs(ray.direction.x) < 0.000001f)
	{
		//若射线方向矢量的x轴分量为0且原点不在盒体内
		if (ray.origin.x > max_bounding.x || ray.origin.x < min_bounding.x)
			return false;
	}
	else
	{
		if (ray.direction.x >= 0)
		{
			tx_min = (min_bounding.x - ray.origin.x) / ray.direction.x;
			tx_max = (max_bounding.x - ray.origin.x) / ray.direction.x;
		}
		else
		{
			tx_min = (max_bounding.x - ray.origin.x) / ray.direction.x;
			tx_max = (min_bounding.x - ray.origin.x) / ray.direction.x;
		}

	}


	if (abs(ray.direction.y) < 0.000001f)
	{
		//若射线方向矢量的x轴分量为0且原点不在盒体内
		if (ray.origin.y > max_bounding.y || ray.origin.y < min_bounding.y)
			return false;
	}
	else
	{
		if (ray.direction.y >= 0)
		{
			ty_min = (min_bounding.y - ray.origin.y) / ray.direction.y;
			ty_max = (max_bounding.y - ray.origin.y) / ray.direction.y;
		}
		else
		{
			ty_min = (max_bounding.y - ray.origin.y) / ray.direction.y;
			ty_max = (min_bounding.y - ray.origin.y) / ray.direction.y;
		}

	}


	if (abs(ray.direction.z) < 0.000001f)
	{
		//若射线方向矢量的x轴分量为0且原点不在盒体内
		if (ray.origin.z > max_bounding.z || ray.origin.z < min_bounding.z)
			return false;
	}
	else
	{
		if (ray.direction.z >= 0)
		{
			tz_min = (min_bounding.z - ray.origin.z) / ray.direction.z;
			tz_max = (max_bounding.z - ray.origin.z) / ray.direction.z;
		}
		else
		{
			tz_min = (max_bounding.z - ray.origin.z) / ray.direction.z;
			tz_max = (min_bounding.z - ray.origin.z) / ray.direction.z;
		}

	}

	double t0, t1;

	//光线进入平面处（最靠近的平面）的最大t值 
	t0 = max(tz_min, max(tx_min, ty_min));

	//光线离开平面处（最远离的平面）的最小t值
	t1 = min(tz_max, min(tx_max, ty_max));
	if (t1 < 0 || t0 > t1 || t0 > ray.ray_max || t1 < ray.ray_min)
		return false;
	return true;


	
}