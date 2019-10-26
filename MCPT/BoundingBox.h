#ifndef __AABB_H
#define __AABB_H

#include "Ray.h"
#include "utlis.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(Vec3f min_b, Vec3f max_b);
	~BoundingBox();
    
    BoundingBox expandbox(BoundingBox box_);   //合并2个AABB
    bool boundingbox_intersect(Ray& ray, Intersection& intersection);//计算ray与AABB交点

    Vec3f min_bounding;
	Vec3f max_bounding;
};

#endif