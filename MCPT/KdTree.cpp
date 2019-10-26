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

//����kdtree
void KdTree::build_kdtree(vector<Triangle*>& triangles_)
{
	//Ҷ�ӽڵ�
	//ֻ�е�����Ƭ����������Ƭ
    if (triangles_.size() <= 2) {
		triangles = triangles_;
		left = NULL;
        right = NULL;
		kd_bbox = triangles_[0]->bbox;
		for(int i = 1;i < triangles_.size();i++)   //������Ƭ��ʱ��Ҫ��չbox
			kd_bbox.expandbox(triangles_[i]->bbox);
    }
	//�ݹ齨��kdtree
    else 
	{
		float minVal, maxVal,pivot;
		minVal = FLT_MAX;
		maxVal = -FLT_MAX;
		//ȡ����������Ƭ�����ĵ���Ϊ����kdtree�����ӽڵ������
		for ( int i = 0; i < triangles_.size(); i++)
		{
			minVal = min(minVal, triangles_[i]->bbox.min_bounding.num[split_axis]);
			maxVal = max(maxVal, triangles_[i]->bbox.max_bounding.num[split_axis]);
		}
		pivot = (maxVal + minVal) / 2;
		
		//������Ƭ�������ӽڵ���
		vector<Triangle*> left_tris;
        vector<Triangle*> right_tris;
		for ( int i = 0; i < triangles_.size(); i++)
		{
			if (triangles_[i]->center_p.num[split_axis] < pivot)
				left_tris.push_back(triangles_[i]);
			else right_tris.push_back(triangles_[i]);
		}
		//�������
		//���������Ƭ������ڵ�/�ҽڵ��£�������ȡ��Ƭ�����ĵ�ֵ���л���
		if (right_tris.size() == triangles_.size())   //������Ƭ���ҽڵ���
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
		else if (left_tris.size() == triangles_.size())  //������Ƭ����ڵ���
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
        //�ݹ齨��kdtree������չ��ǰ�ڵ��boxΪ���ҽڵ�ĺϲ�box
		left = new KdTree(left_tris);
		right = new KdTree(right_tris);
		kd_bbox = left->kd_bbox.expandbox(right->kd_bbox);
		
    }
}

//kdtree����ߵ���
bool KdTree::kdtree_intersect(Ray& ray, Intersection& intersection)
{
    //�ݹ���

	bool left_intersect = false;
	bool right_intersect = false;
	bool hit = false;

   //�뵱ǰ�ڵ��box���ཻ�㣬����ڵ��µ�������Ƭ�������ཻ
    if (!kd_bbox.boundingbox_intersect(ray, intersection)) 
		return false;

	//����Ѿ��󽻵�����Ҷ�ӽڵ�
	if (left == NULL && right == NULL)
	{
		//�Խڵ��µ�ÿ����Ƭ������������Ƭ����ߵ��󽻼���
		for (int i = 0; i < triangles.size(); i++)
			if (triangles[i]->tri_intersect(ray, intersection))
				return true;
	}
	//������ݹ���������ӽڵ������
	else {
		left_intersect = left->kdtree_intersect(ray,intersection);
		right_intersect = right->kdtree_intersect(ray,intersection);
		return left_intersect || right_intersect;
	}

    return false;
}


