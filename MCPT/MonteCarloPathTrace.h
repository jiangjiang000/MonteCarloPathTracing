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

	void setCamera(Vec3f position_, Vec3f center_, Vec3f up_, float fov_, int w, int h);//�����������
	Ray generate_ray(float x, float y);                                   //����һ����ʼ����
	Ray montecarlo_sampling(Ray ray, Intersection intersection);          //�ڽ��㴦�������ؿ�����������������ߵķ���
	Vec3f trace_ray(Ray ray, int depth);                                  //����׷��
	Vec3f compute_direction_illum(Ray &ray, Intersection &intersection);  //�����Խ��㴦��Ӱ��

    float* run();

	Model* model;//��Ҫ�ⲿ����model
	Camera camera; //���
	KdTree* kdTree; //kdtree
	vector<Light> lights;//��Դ��Ҫ������������,����ֱ�ӹ���
	float* colors;//�������ڵ�color����RayTracer�м���

	int width, height;
	int it_num;    //��ǰ��������
	int max_it_num;//���ؿ��������Ŀ
	int max_depth;//�������ݹ����
	Vec3f ambient; //��������

	clock_t begin, end;
};

#endif