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
	void build_kdtree(vector<Triangle*>& triangles_);                       //����kdtree
	bool kdtree_intersect(Ray& ray, Intersection& intersection);           //kdtree����ߵ���

	KdTree *left, *right;      //kdtree�������ӽڵ�
	int leftnum, rightnum;     //kdtree�����ӽڵ������������Ƭ����
	vector<Triangle*> triangles; //������Ƭ
	BoundingBox kd_bbox;        //��Χ��
	int split_axis = 2;

};

#endif