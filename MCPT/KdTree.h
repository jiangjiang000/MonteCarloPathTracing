#ifndef __KDTREE_H
#define __KDTREE_H
#include "BoundingBox.h"
#include"Model.h"
#include <vector>

class Triangle;
class Model;
class KdTree;

class KdTree 
{
public:

    KdTree();
    KdTree(vector<Triangle*>& triangles_);
	~KdTree();
	void build_kdtree(vector<Triangle*>& triangles_);                       //建立kdtree
	bool kdtree_intersect(Ray& ray, Intersection& intersection);           //kdtree与光线的求交

	KdTree *left, *right;      //kdtree的左右子节点
	int leftnum, rightnum;     //kdtree左右子节点包含的三角面片总数
	vector<Triangle*> triangles; //三角面片
	BoundingBox kd_bbox;        //包围盒
	int split_axis = 2;

};

#endif