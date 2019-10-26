#ifndef __RAY_H
#define __RAY_H

#include "utlis.h"


class Ray
{
public:
	Ray();
	Ray(Vec3f o, Vec3f d, string t = "none");
	~Ray();

	void reflect(Vec3f& normal, Vec3f &dir);   //������������ʱҪ���㷴�����
	bool isRefract(Vec3f normal, double ratio, Vec3f &dir); //����������ʱ���ж��Ƿ񻹻ᷢ������

	Vec3f origin;             //���ߵ���ʼ�㣬һ��Ϊ���λ��
	Vec3f direction;          //���ߵķ���
	string type;              //���ߵ����ͣ�һ����none,diffuse,specular,refraction
	float ray_min, ray_max;   //�����ܵ�����������Զ���룬ray_max�ڲ��ϸ���
};

class Light
{
public:
	Vec3f center;               //��Դλ��
	Vec3f dx, dy;               //��Դ����
	Vec3f normal;               //��Դ����
	Vec3f emission;             //��Դ�����
	float area;                 //�������
	int lightSamples;           //������
	string type;                //���ͣ���diffuse��specular��refraction
	int factor = 2;

	Light();
	Light(Vec3f c, float r_, Vec3f e, string type, float f);
	Light(Vec3f c, Vec3f dx_, Vec3f dy_, Vec3f e, string type = "none", float r_ = 0.0, float f = 2.0);

};

#endif