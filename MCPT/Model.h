#ifndef __MODEL_H
#define __MODEL_H

#include <vector>
#include <string>

#include "utlis.h"
#include "BoundingBox.h"
#include "KdTree.h"



class Model;
class Triangle;
class KdTree;

class Triangle
{
public:
	int v_idx[3];    //��������
	int vn_idx[3];   //��������
	Vec3f v[3];      //��������
	Vec3f vn[3];     //��������
	Vec3f e1, e2, center_p;    //������Ƭ�����������ߺ����ĵ�
	Vec3f fn;        //�淨��
	BoundingBox bbox;  //������Ƭ�İ�Χ��
	Material t_material;  //����

    Model* model;//������Ӧ��model

    Triangle(int v_[3], int vn_[3], Model *m);
	~Triangle();

    
    bool tri_intersect(Ray& ray, Intersection& intersection);  //������Ƭ����ߵ���
	Vec3f get_inter_point_normal(Vec3f point);                 //��������Ƭ��ĳ��ķ���
};


class Model
{
public:
	vector<Vec3f> vertexs;      //����
	vector<Vec3f> normals;      //���㷨��
	vector<Material> materials; //���ʱ�
	vector<Triangle*> triangles; //�桪��������Ƭ
   
	BoundingBox bbox;           //��Χ��
    KdTree *kdtree;

    Model();
    ~Model();

    bool LoadModel(string filename);      //����ģ��
	bool LoadMaterial(string filename);   //���ض�Ӧ�Ĳ����ļ�
	Material findMaterial(string name);  //�Ӳ��ʱ����ҵ���Ӧ���ƵĲ���

};

#endif