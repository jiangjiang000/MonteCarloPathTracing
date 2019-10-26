#include "KdTree.h"
#include <algorithm>

using namespace std;

KdTree::KdTree()
{

}

KdTree::KdTree(vector<Triangle*>& triangles_)
{
	build_kdtree(triangles_);
}

KdTree::~KdTree()
{

}

//建立kdtree
void KdTree::build_kdtree(vector<Triangle*>& triangles_)
{
	//叶子节点
	//只有单个面片或者两个面片
    if (triangles_.size() <= 2) {
		triangles = triangles_;
		left = NULL;
        right = NULL;
		kd_bbox = triangles_[0]->bbox;
		for(int i = 1;i < triangles_.size();i++)   //两个面片的时候要扩展box
			kd_bbox.expandbox(triangles_[i]->bbox);
    }
	//递归建立kdtree
    else 
	{
		float minVal, maxVal,pivot;
		minVal = FLT_MAX;
		maxVal = -FLT_MAX;
		//取所有三角面片的中心点作为划分kdtree左右子节点的依据
		for ( int i = 0; i < triangles_.size(); i++)
		{
			minVal = min(minVal, triangles_[i]->bbox.min_bounding.num[split_axis]);
			maxVal = max(maxVal, triangles_[i]->bbox.max_bounding.num[split_axis]);
		}
		pivot = (maxVal + minVal) / 2;
		
		//划分面片到左、右子节点下
		vector<Triangle*> left_tris;
        vector<Triangle*> right_tris;
		for ( int i = 0; i < triangles_.size(); i++)
		{
			if (triangles_[i]->center_p.num[split_axis] < pivot)
				left_tris.push_back(triangles_[i]);
			else right_tris.push_back(triangles_[i]);
		}
		//极端情况
		//如果所有面片都在左节点/右节点下，则重新取面片的中心点值进行划分
		if (right_tris.size() == triangles_.size())   //所有面片在右节点下
		{
			float mintmp = FLT_MAX;
			unsigned int index;
			for ( int i = 0; i < right_tris.size(); i++)
			{
				
				if (right_tris[i]->center_p.num[split_axis] < mintmp)
				{
					mintmp = right_tris[i]->center_p.num[split_axis];
					index = i;
				}
			}
			left_tris.push_back(right_tris[index]);
			right_tris.erase(right_tris.begin() + index);
		}
		else if (left_tris.size() == triangles_.size())  //所有面片在左节点下
		{
			float maxtmp = -FLT_MAX;
			unsigned int index;
			for (int i = 0; i < left_tris.size(); i++)
			{
				if (left_tris[i]->center_p.num[split_axis] > maxtmp)
				{
					maxtmp = left_tris[i]->center_p.num[split_axis];
					index = i;
				}
			}
			right_tris.push_back(left_tris[index]);
			left_tris.erase(left_tris.begin() + index);
			

		}
        //递归建立kdtree，并扩展当前节点的box为左右节点的合并box
		left = new KdTree(left_tris);
		right = new KdTree(right_tris);
		kd_bbox = left->kd_bbox.expandbox(right->kd_bbox);
		
    }
}

//kdtree与光线的求交
bool KdTree::kdtree_intersect(Ray& ray, Intersection& intersection)
{
    //递归求交

	bool left_intersect = false;
	bool right_intersect = false;
	bool hit = false;

   //与当前节点的box无相交点，则与节点下的所有面片均不会相交
    if (!kd_bbox.boundingbox_intersect(ray, intersection)) 
		return false;

	//如果已经求交到最后的叶子节点
	if (left == NULL && right == NULL)
	{
		//对节点下的每个面片都进行三角面片与光线的求交计算
		for (int i = 0; i < triangles.size(); i++)
			if (triangles[i]->tri_intersect(ray, intersection))
				return true;
	}
	//否则，则递归进入左右子节点进行求交
	else {
		left_intersect = left->kdtree_intersect(ray,intersection);
		right_intersect = right->kdtree_intersect(ray,intersection);
		return left_intersect || right_intersect;
	}

    return false;
}


