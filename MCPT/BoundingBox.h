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
    
    BoundingBox expandbox(BoundingBox box_);   //�ϲ�2��AABB
    bool boundingbox_intersect(Ray& ray, Intersection& intersection);//����ray��AABB����

    Vec3f min_bounding;
	Vec3f max_bounding;
};

#endif