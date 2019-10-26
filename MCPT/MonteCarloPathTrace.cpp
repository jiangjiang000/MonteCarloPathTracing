#include "MonteCarloPathTrace.h"
#include <omp.h>
#include <iostream>
#include <string>


using namespace std;


MonteCarloPathTrace::MonteCarloPathTrace()
{
	width = 0;
	height = 0;
	colors = NULL;
}

MonteCarloPathTrace::MonteCarloPathTrace(Model* model_,int w_,int h_)
{
	model = model_;
	width = w_;
	height = h_;
	kdTree = model_->kdtree;

	it_num = 0;
	max_it_num = 100;
	max_depth = 6;
    ambient = Vec3f(0.2, 0.2, 0.2);
	
	if (colors != NULL)
		delete[] colors;
	colors = new float[3 * width*height];


}

MonteCarloPathTrace::~MonteCarloPathTrace()
{
	if (colors != NULL)
	{
		delete[] colors;
		colors = NULL;
	}
}

//�����������
void MonteCarloPathTrace::setCamera(Vec3f position_, Vec3f lookat_, Vec3f up_, float fov_, int w, int h)
{
	camera.position = position_;
	camera.lookat = lookat_ - position_;
	//camera.lookat = lookat_;
	camera.up = up_;
	camera.fovy = fov_;
	camera.win_w = w;
	camera.win_h = h;

	Vec3f tmp = camera.lookat.cross(camera.up);
	camera.up = tmp.cross(camera.lookat);

	camera.lookat.normalize();
	tmp.normalize();
	camera.up.normalize();

	float ratio = (float) h / (float) w;

	//������Ļ��С���������ܿ����ļ���λ��
	camera.view_x = tmp * (tan(fov_ * PI / 360) * 2);
	camera.view_y = camera.up * (tan(fov_ * ratio * PI / 360) * 2);
	camera.view_z = camera.lookat;
}

// �������һ�������߷����Ĺ���
Ray MonteCarloPathTrace::generate_ray(float x, float y)
{
	//����x��y�����ϵ����������������������߷���
	srand(int(time(0)));
	float rx = (float)rand() / RAND_MAX;
	float ry = (float)rand() / RAND_MAX;

	Vec3f direction = Vec3f(camera.view_z + camera.view_x * ((x + rx) / width - 0.5f)) + camera.view_y * ((y + ry) / height - 0.5f);

	//�����Ĺ��������߷������������ȷ��
	Ray ray(camera.position, direction, "none");
	return ray;
}


//�ڽ��㴦�������ؿ�����������������ߵķ���
Ray MonteCarloPathTrace::montecarlo_sampling(Ray ray, Intersection intersection)
{
	Material inter_material = intersection.material;
	Vec3f new_dir;  //�²������ߵķ���
	float r,r1,r2;        //�����
	//����������ڵ������
	//�ж�
	if (inter_material.Ni > 1.01f)
	{
		Vec3f normal;
		float ni, nt;
		//���ݹ��߷����뽻�㴦�ķ���ĵ����ȷ������ķ���
		float cosin = ray.direction.dot(intersection.normal);
		//�������Ǵ������ڵ�
		if (cosin > 0)
		{
			normal = intersection.normal * -1.0;
			ni = inter_material.Ni;
			nt = 1.0;
		}
		//�������Ǵ��������
		else
		{
			normal = intersection.normal;
			ni = 1.0;
			nt = inter_material.Ni;
		}
		//����fresnel��ϵ��
		float f = ((ni - nt) / (ni + nt)) * ((ni - nt) / (ni + nt));
		f = f + (1 - f) * pow(1 - abs(cosin), 5);
		//�����������Ϊ��ֵ�ж�
		//srand(int(time(0)));
		r = (float)rand() / RAND_MAX;
		//���fresnel��ϵ��С�����������˵㴦�����������������������������ǽ������仹�Ƿ���
		if (f < r)
		{
			//���ݹ��ߵ����乫ʽ�ж��Ƿ���������ڣ����������������
			if (ray.isRefract(normal, ni / nt, new_dir))
				return Ray(intersection.point, new_dir, "refraction");
			else  //û�����䣬����з���
			{
				ray.reflect(normal, new_dir);
				return Ray(intersection.point, new_dir, "specular");
			}
		}

	}

	//��������
	//�������������Ϊ��ֵ�ж�
	//srand(int(time(0)));
	float theta, phi;
	Vec3f dir;
	r = (float)rand() / RAND_MAX;
	r1 = (float)rand() / RAND_MAX;
	r2 = (float)rand() / RAND_MAX;
	phi = r1 * 2 * PI;

	float diffuse, specular;
	diffuse = inter_material.Kd.dot(Vec3f(1.0f, 1.0f, 1.0f));
	specular = inter_material.Ks.dot(Vec3f(1.0f, 1.0f, 1.0f)) + diffuse;
	//���ks����kd������о��淴��
	if ((diffuse / specular) <= r)
	{
		Vec3f tmp_dir;
		ray.reflect(intersection.normal, tmp_dir);  //�õ�һ��������
		theta = acos(pow(r2, 1 / (inter_material.Ns + 1)));
		dir = tmp_dir;
	}
	//�������������
	else
	{
		theta = asin(sqrt(r2));
		dir = intersection.normal;
	}
	//��ǰһ�ι��ߺͱ�����ʵ�Լ���£���������õ�һ���µķ���
	Vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
	Vec3f front;
	if (fabs(dir.x) > fabs(dir.y)) {
		front = Vec3f(dir.z, 0, -dir.x);
		front.normalize();
	}
	else {
		front = Vec3f(0, -dir.z, dir.y);
		front.normalize();
	}
	Vec3f right = dir.cross(front);
	new_dir = (Vec3f(Vec3f(right * sample.x) + dir * sample.y) + front * sample.z);
	new_dir.normalize();

	//���ع���
	if((diffuse / specular) <= r)
		return Ray(intersection.point, new_dir, "specular");
	else
		return Ray(intersection.point, new_dir, "diffuse");
}

//�����Խ��㴦��Ӱ��
Vec3f MonteCarloPathTrace::compute_direction_illum(Ray &ray, Intersection &intersection)
{
	Vec3f final_color;
	//�������̲߳��У���������
    #pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0;i < lights.size();i++)
	{
		Vec3f tmp_color;
		Light light = lights[i];
		for (int j = 0; j < light.lightSamples; j++)
		{
			float rx, ry;  //�����,����x�����y�����ϵ�С���벽��
			float theta, phi;
			//srand(int(time(0)));
			rx = (float)rand() / RAND_MAX;
			ry = (float)rand() / RAND_MAX;
			Vec3f new_direction = Vec3f(light.center + light.dx * rx) + light.dy * ry - intersection.point;
			Ray new_light(intersection.point, new_direction); //����һ���ӽ��㷢����ָ���Դ�Ĺ���
			new_light.ray_max = new_direction.length();

			//�¹����볡���󽻲���
			Intersection tmp_inter;
			
			//����ӹ�Դ�����Ĺ������ཻ��֮�䲻�����������¹����ཻ������
			//���������ڵ�������£��ཻ����ɫ���ܹ�ԴӰ��
			if (!kdTree->kdtree_intersect(new_light, tmp_inter))  
			{
				//�����Դ�Խ��㴦��Ӱ��ϵ�����������Դ��˥���̶�
				new_direction.normalize();
				float cosin_N_L = intersection.normal.dot(new_direction);
				float cosin_LN_L = Vec3f(new_direction * (-1)).dot(light.normal);
				float solid_angle = cosin_LN_L * light.area / (pow(new_light.ray_max, 2) * light.lightSamples) / (2 * PI);
				
				if (intersection.material.Kd != Vec3f(0.0f, 0.0f, 0.0f) && cosin_N_L > 0.0)
				{
					//�ܹ��յ�Ӱ���Ľ��㴦����ɫ
					tmp_color = tmp_color + Vec3f(light.emission * solid_angle * cosin_N_L * intersection.material.Kd);
				}
			}

		}
		final_color = final_color + tmp_color;
	}
	return final_color;
}

Vec3f MonteCarloPathTrace::trace_ray(Ray ray, int depth)
{
	Intersection intersection;
	Vec3f indir_c, dir_c, final_c;

	//�󽻼��
	if (!kdTree->kdtree_intersect(ray, intersection))
		return indir_c;
	
	Material inter_material = intersection.material;
	//����ݹ鵽���õ�������ʱ����ֱ�ӷ����ཻ�㴦���ʵķ�����ɫ
	if (depth >= max_depth)
		return inter_material.Ke;
	//�ڽ��㴦�������ؿ��巽������һ���������
	Ray new_ray = montecarlo_sampling(ray, intersection);

	if (new_ray.type != "none") {
		indir_c = trace_ray(new_ray, depth + 1); //�ݹ���й���׷�٣�ֱ��û�н��㴦

		//�����ɵĹ��ߵ����Ͳ�ͬ������ͬ����
		if (new_ray.type == "diffuse")
		{
			indir_c = inter_material.Kd * indir_c;
		}
		else if (new_ray.type == "specular")
		{
			indir_c = inter_material.Ks * indir_c;
		}
		else if (new_ray.type == "refraction")
		{
			indir_c =  inter_material.Tf * indir_c;
		}
	}
	//��Դ���ཻ�������ڵ���������ɫ���ܹ�Դ����Ӱ��
	if (lights.size() != 0)
		dir_c = compute_direction_illum(ray, intersection);
	
	final_c = Vec3f( Vec3f(inter_material.Ke + indir_c) + dir_c) + inter_material.Ka * ambient;
	return final_c;
}



float* MonteCarloPathTrace::run()
{
	begin = clock();
	it_num++;
    if (it_num > max_it_num)
        return colors;

    //�������̲߳��У���������
    #pragma omp parallel for schedule(dynamic, 1)
    for (int w = 0; w<width; w++)
    {
        for (int h = 0; h<height; h++)
        {
			Vec3f color;
            Ray rays = generate_ray(w, h);//��x��y�ӽǸ����������һϵ�й���
			color = trace_ray(rays,0);   //���й���׷��
               
			if (color.r < 0.0f)color.r = 0.0f;
            if (color.b < 0.0f)color.b = 0.0f;
            if (color.g < 0.0f)color.g = 0.0f;

            if (color.r > 1.0f)color.r = 1.0f;
            if (color.b > 1.0f)color.b = 1.0f;
            if (color.g > 1.0f)color.g = 1.0f;

            int index = h*width * 3 + w * 3;
          
			//��ÿ�ε�������ɫ����ȡ��ֵ����
			colors[index] = (colors[index] * (it_num - 1) + color.r) / it_num;
			colors[index + 1] = (colors[index + 1] * (it_num - 1) + color.g) / it_num;
			colors[index + 2] = (colors[index + 2] * (it_num - 1) + color.b) / it_num;

        }
    }
	end = clock();
	cout << "the " << it_num << "'th iteration, cost time: " << (float)(end - begin)/1000  << "'s";
	if (it_num % 10 == 1 && it_num <= 101)
		cout << " , save pic";
	cout << endl;
    return colors;
}


