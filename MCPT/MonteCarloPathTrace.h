#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "utlis.h"
#include "Ray.h"
#include"Model.h"
#include<algorithm>
#include <ctime>

class MonteCarloPathTrace
{
public:
	MonteCarloPathTrace();
	MonteCarloPathTrace(Model* model_, int w_, int h_);
	~MonteCarloPathTrace();

	void setCamera(Vec3f position_, Vec3f center_, Vec3f up_, float fov_, int w, int h);//设置相机参数
	Ray generate_ray(float x, float y);                                   //生成一条初始光线
	Ray montecarlo_sampling(Ray ray, Intersection intersection);          //在交点处，用蒙特卡洛随机采样产生光线的方向
	Vec3f trace_ray(Ray ray, int depth);                                  //光线追踪
	Vec3f compute_direction_illum(Ray &ray, Intersection &intersection);  //照明对交点处的影响

    float* run();

	Model* model;//需要外部输入model
	Camera camera; //相机
	KdTree* kdTree; //kdtree
	vector<Light> lights;//光源需要在类外进行添加,用于直接光照
	float* colors;//整个窗口的color，在RayTracer中计算

	int width, height;
	int it_num;    //当前迭代次数
	int max_it_num;//蒙特卡洛采样数目
	int max_depth;//光线最大递归深度
	Vec3f ambient; //环境光照

	clock_t begin, end;
};

#endif