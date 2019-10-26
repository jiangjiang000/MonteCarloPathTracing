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
	int v_idx[3];    //顶点索引
	int vn_idx[3];   //法向索引
	Vec3f v[3];      //顶点坐标
	Vec3f vn[3];     //法向坐标
	Vec3f e1, e2, center_p;    //三角面片的其中两个边和中心点
	Vec3f fn;        //面法向
	BoundingBox bbox;  //三角面片的包围盒
	Material t_material;  //材质

    Model* model;//面所对应的model

    Triangle(int v_[3], int vn_[3], Model *m);
	~Triangle();

    
    bool tri_intersect(Ray& ray, Intersection& intersection);  //三角面片与光线的求交
	Vec3f get_inter_point_normal(Vec3f point);                 //求三角面片中某点的法向
};


class Model
{
public:
	vector<Vec3f> vertexs;      //顶点
	vector<Vec3f> normals;      //顶点法向
	vector<Material> materials; //材质表
	vector<Triangle*> triangles; //面——三角面片
   
	BoundingBox bbox;           //包围盒
    KdTree *kdtree;

    Model();
    ~Model();

    bool LoadModel(string filename);      //加载模型
	bool LoadMaterial(string filename);   //加载对应的材质文件
	Material findMaterial(string name);  //从材质表中找到对应名称的材质

};

#endif