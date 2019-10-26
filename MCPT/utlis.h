#ifndef __VEC_H
#define __VEC_H

#include <cmath>
#include<iostream>

using namespace std;

const float PI = 3.1415926f;

class Vec3f
{
public:
	union {
		struct{ float x, y, z; };
		struct{ float r, g, b; };
        float num[3];
	};

	Vec3f() { this->x = 0; this->y = 0; this->z = 0; };
	Vec3f(float x, float y, float z) { this->x = x; this->y = y; this->z = z; };

    inline Vec3f operator - () { return Vec3f(-x, -y, -z); }

	inline Vec3f& operator = (const Vec3f &v) { x = v.x; y = v.y; z = v.z; return *this; }

	inline Vec3f& operator += (float num) { x += num; y += num; z += num; return *this; }
	inline Vec3f& operator += (const Vec3f &v) { x += v.x; y += v.y; z += v.z; return *this; }

	inline Vec3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
	inline Vec3f& operator -= (const Vec3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	inline Vec3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
	inline Vec3f& operator *= (const Vec3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

	inline Vec3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
	inline Vec3f& operator /= (const Vec3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }


    friend inline Vec3f operator + (const Vec3f &u, float num) { return Vec3f(u.x + num, u.y + num, u.z + num); }
    friend inline Vec3f operator + (float num, const Vec3f &u) { return Vec3f(num + u.x, num + u.y, num + u.z); }
	friend inline Vec3f operator + (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
    friend inline Vec3f operator - (const Vec3f &u, float num) { return Vec3f(u.x - num, u.y - num, u.z - num); }
    friend inline Vec3f operator - (float num, const Vec3f &u) { return Vec3f(num - u.x, num - u.y, num - u.z); }
	friend inline Vec3f operator - (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline Vec3f operator * (const Vec3f &u, float num) { return Vec3f(u.x * num, u.y * num, u.z * num); }
	friend inline Vec3f operator * (float num, const Vec3f &u) { return Vec3f(num * u.x, num * u.y, num * u.z); }
	friend inline Vec3f operator * (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline Vec3f operator / (const Vec3f &u, float num) { return Vec3f(u.x / num, u.y / num, u.z / num); }
	friend inline Vec3f operator / (float num, const Vec3f &u) { return Vec3f(num / u.x, num / u.y, num / u.z); }
	friend inline Vec3f operator / (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	inline bool operator == (const Vec3f &u) { return x == u.x && y== u.y && z==u.z; }
	inline bool operator != (const Vec3f &u) { return !(x == u.x && y == u.y && z == u.z); }

	inline float dot( const Vec3f &v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline Vec3f cross(const Vec3f &v)
	{
		return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	inline float length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline void normalize()
	{
		float mod = (float)sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
		this->x /= mod;
		this->y /= mod;
		this->z /= mod;
		
	}
};

struct Material
{
	string material_name;
	Vec3f Ke;                   //������ɫ
	float Ns = 60;              //�����
	float Ni = 1.0;             //����ϵ��
	Vec3f Tf;                   //Ͷ���˲�
	Vec3f Ka;                   //������ϵ��
	Vec3f Kd;                   //������ϵ��
	Vec3f Ks;                   //���淴��ϵ��

};

struct Intersection
{
	Material material;          //���㴦����
	Vec3f point;                //��������
	Vec3f normal;               //��������λ�õķ���

};

struct Camera
{
	Vec3f position;            //���λ��
	Vec3f lookat;              //�������
	Vec3f up;                  //����Ϸ���
	float fovy = 0;            //�ӳ���

	Vec3f view_x, view_y, view_z;  //�����ܿ�������Զ����
	int win_w, win_h;              //��ʾ��Ļ����
};



#endif