#ifndef __RAY_H
#define __RAY_H

#include "utlis.h"


class Ray
{
public:
	Ray();
	Ray(Vec3f o, Vec3f d, string t = "none");
	~Ray();

	void reflect(Vec3f& normal, Vec3f &dir);   //当光线遇镜面时要计算反射光线
	bool isRefract(Vec3f normal, double ratio, Vec3f &dir); //光线遇表面时，判断是否还会发生折射

	Vec3f origin;             //光线的起始点，一般为相机位置
	Vec3f direction;          //光线的方向
	string type;              //光线的类型，一般有none,diffuse,specular,refraction
	float ray_min, ray_max;   //光线能到达的最近和最远距离，ray_max在不断更新
};

class Light
{
public:
	Vec3f center;               //光源位置
	Vec3f dx, dy;               //光源方向
	Vec3f normal;               //光源法向
	Vec3f emission;             //光源辐射度
	float area;                 //照射面积
	int lightSamples;           //采样数
	string type;                //类型，有diffuse，specular，refraction
	int factor = 2;

	Light();
	Light(Vec3f c, float r_, Vec3f e, string type, float f);
	Light(Vec3f c, Vec3f dx_, Vec3f dy_, Vec3f e, string type = "none", float r_ = 0.0, float f = 2.0);

};

#endif