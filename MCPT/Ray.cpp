#include "Ray.h"

Ray::Ray()
{
	origin = Vec3f(0.0, 0.0, 0.0);
	direction = Vec3f(0.0, 0.0, 0.0);
	type = "none";
	ray_min = -0xfffffff;
	ray_max = 0xfffffff;
}


Ray::Ray(Vec3f o, Vec3f d, string t)
{
	origin = o;
	direction = d;
	direction.normalize();
	type = t;
	ray_min = 1e-5f;
	ray_max = 0xfffffff;
}

Ray::~Ray()
{
}


//������������ʱҪ���㷴�����
void Ray::reflect(Vec3f& normal,Vec3f &dir)
{ 
	//���ݹ�ʽ R = 2(N��L)N - L ������
	//L�ǹ��ߵ�����������N�Ƿ��ߣ�R�Ƿ�������
	Vec3f N_L_N = normal * normal.dot(direction);
	Vec3f tmp = N_L_N * 2.0f;
	dir = direction - tmp;
}

//����������ʱ���ж��Ƿ񻹻ᷢ������
bool Ray::isRefract(Vec3f normal, double ratio, Vec3f &dir)
{
	float N_L, k;
	N_L = normal.dot(direction);
	k = 1.0 - ratio * ratio * (1.0 - N_L * N_L);
	if (k >= 0.0f)
	{
		dir = Vec3f(direction * ratio) - Vec3f(normal * (ratio * N_L + sqrt(k)));
		return true;
	}
	return false;
}

Light::Light()
{

}

Light::Light(Vec3f c, float r_, Vec3f e, string type, float f) {

	center = c;
	emission = e;
	lightSamples = 3;

	dx = Vec3f(0, 0, r_ * f * -2);    //���˸�ϵ������Ȼ�������Ч���ܰ�
	dy = Vec3f(r_ * f, 0, r_ * f);
	normal = dx.cross(dy);
	if (type == "sphere")             //��Դ������Ļ���������ܰ뾶Ӱ��
		area = dx.length()*dy.length()* PI *f;
	else
		area = normal.length();
	normal.normalize();
}
Light::Light(Vec3f c, Vec3f dx_, Vec3f dy_, Vec3f e, string type, float r_, float f ) {
	center = c;
	emission = e;
	lightSamples = 3;
	if (type == "sphere")    //��Դ������
	{
		dx = Vec3f(0, 0, r_ * f * -2);
		dy = Vec3f(r_ *f, 0, r_*f);
		normal = dx.cross(dy);
		area = dx.length() * dy.length() * PI * f * 2;
	}
	else                    //��Ȼ���Ƿ��ι�Դ
	{
		dx = dx_;
		dy = dy_;
		normal = dx.cross(dy);
		area = normal.length();
	}
	normal.normalize();
}