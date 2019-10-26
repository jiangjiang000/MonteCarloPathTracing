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

//设置相机参数
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

	//根据屏幕大小计算视线能看到的极限位置
	camera.view_x = tmp * (tan(fov_ * PI / 360) * 2);
	camera.view_y = camera.up * (tan(fov_ * ratio * PI / 360) * 2);
	camera.view_z = camera.lookat;
}

// 随机产生一条从视线发出的光线
Ray MonteCarloPathTrace::generate_ray(float x, float y)
{
	//产生x和y方向上的随机数，用于随机产生光线方向
	srand(int(time(0)));
	float rx = (float)rand() / RAND_MAX;
	float ry = (float)rand() / RAND_MAX;

	Vec3f direction = Vec3f(camera.view_z + camera.view_x * ((x + rx) / width - 0.5f)) + camera.view_y * ((y + ry) / height - 0.5f);

	//产生的光线由视线发出，方向随机确定
	Ray ray(camera.position, direction, "none");
	return ray;
}


//在交点处，用蒙特卡洛随机采样产生光线的方向
Ray MonteCarloPathTrace::montecarlo_sampling(Ray ray, Intersection intersection)
{
	Material inter_material = intersection.material;
	Vec3f new_dir;  //新采样光线的方向
	float r,r1,r2;        //随机数
	//还有折射存在的情况下
	//判断
	if (inter_material.Ni > 1.01f)
	{
		Vec3f normal;
		float ni, nt;
		//根据光线方向与交点处的法向的点积，确定折射的方向
		float cosin = ray.direction.dot(intersection.normal);
		//此折射是从外往内的
		if (cosin > 0)
		{
			normal = intersection.normal * -1.0;
			ni = inter_material.Ni;
			nt = 1.0;
		}
		//此折射是从内往外的
		else
		{
			normal = intersection.normal;
			ni = 1.0;
			nt = inter_material.Ni;
		}
		//计算fresnel项系数
		float f = ((ni - nt) / (ni + nt)) * ((ni - nt) / (ni + nt));
		f = f + (1 - f) * pow(1 - abs(cosin), 5);
		//生成随机数作为阈值判断
		//srand(int(time(0)));
		r = (float)rand() / RAND_MAX;
		//如果fresnel项系数小于随机数，则此点处还存在折射情况，但还需具体讨论是进行折射还是反射
		if (f < r)
		{
			//根据光线的折射公式判断是否还有折射存在，有则继续进行折射
			if (ray.isRefract(normal, ni / nt, new_dir))
				return Ray(intersection.point, new_dir, "refraction");
			else  //没有折射，则进行反射
			{
				ray.reflect(normal, new_dir);
				return Ray(intersection.point, new_dir, "specular");
			}
		}

	}

	//反射的情况
	//再生成随机数作为阈值判断
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
	//如果ks大于kd，则进行镜面反射
	if ((diffuse / specular) <= r)
	{
		Vec3f tmp_dir;
		ray.reflect(intersection.normal, tmp_dir);  //得到一个反方向
		theta = acos(pow(r2, 1 / (inter_material.Ns + 1)));
		dir = tmp_dir;
	}
	//否则进行漫反射
	else
	{
		theta = asin(sqrt(r2));
		dir = intersection.normal;
	}
	//在前一段光线和表面材质的约束下，随机采样得到一个新的方向
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

	//返回光线
	if((diffuse / specular) <= r)
		return Ray(intersection.point, new_dir, "specular");
	else
		return Ray(intersection.point, new_dir, "diffuse");
}

//照明对交点处的影响
Vec3f MonteCarloPathTrace::compute_direction_illum(Ray &ray, Intersection &intersection)
{
	Vec3f final_color;
	//开启多线程并行，加速运算
    #pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0;i < lights.size();i++)
	{
		Vec3f tmp_color;
		Light light = lights[i];
		for (int j = 0; j < light.lightSamples; j++)
		{
			float rx, ry;  //随机数,用于x方向和y方向上的小距离步进
			float theta, phi;
			//srand(int(time(0)));
			rx = (float)rand() / RAND_MAX;
			ry = (float)rand() / RAND_MAX;
			Vec3f new_direction = Vec3f(light.center + light.dx * rx) + light.dy * ry - intersection.point;
			Ray new_light(intersection.point, new_direction); //生成一条从交点发出的指向光源的光线
			new_light.ray_max = new_direction.length();

			//新光线与场景求交测试
			Intersection tmp_inter;
			
			//如果从光源发出的光线与相交点之间不存在其他与新光线相交的物体
			//即不存在遮挡的情况下，相交点颜色会受光源影响
			if (!kdTree->kdtree_intersect(new_light, tmp_inter))  
			{
				//计算光源对交点处的影响系数，即计算光源的衰减程度
				new_direction.normalize();
				float cosin_N_L = intersection.normal.dot(new_direction);
				float cosin_LN_L = Vec3f(new_direction * (-1)).dot(light.normal);
				float solid_angle = cosin_LN_L * light.area / (pow(new_light.ray_max, 2) * light.lightSamples) / (2 * PI);
				
				if (intersection.material.Kd != Vec3f(0.0f, 0.0f, 0.0f) && cosin_N_L > 0.0)
				{
					//受光照的影响后的交点处的颜色
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

	//求交检测
	if (!kdTree->kdtree_intersect(ray, intersection))
		return indir_c;
	
	Material inter_material = intersection.material;
	//如果递归到设置的最大深度时，则直接返回相交点处材质的发光颜色
	if (depth >= max_depth)
		return inter_material.Ke;
	//在交点处，用蒙特卡洛方法生成一条随机光线
	Ray new_ray = montecarlo_sampling(ray, intersection);

	if (new_ray.type != "none") {
		indir_c = trace_ray(new_ray, depth + 1); //递归进行光线追踪，直至没有交点处

		//新生成的光线的类型不同，做不同处理
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
	//光源与相交点若无遮挡，则其颜色会受光源照明影响
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

    //开启多线程并行，加速运算
    #pragma omp parallel for schedule(dynamic, 1)
    for (int w = 0; w<width; w++)
    {
        for (int h = 0; h<height; h++)
        {
			Vec3f color;
            Ray rays = generate_ray(w, h);//在x，y视角附近随机产生一系列光线
			color = trace_ray(rays,0);   //进行光线追踪
               
			if (color.r < 0.0f)color.r = 0.0f;
            if (color.b < 0.0f)color.b = 0.0f;
            if (color.g < 0.0f)color.g = 0.0f;

            if (color.r > 1.0f)color.r = 1.0f;
            if (color.b > 1.0f)color.b = 1.0f;
            if (color.g > 1.0f)color.g = 1.0f;

            int index = h*width * 3 + w * 3;
          
			//对每次迭代的颜色进行取均值处理
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


