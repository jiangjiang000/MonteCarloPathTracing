#include "Model.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include "utlis.h"

using namespace std;

Triangle::Triangle(int v_[3], int vn_[3], Model *m)
{
	model = m;

	for (int i = 0; i < 3; i++)
	{
		v_idx[i] = v_[i];
		vn_idx[i] = vn_[i];
		v[i] = model->vertexs[v_idx[i]];
		vn[i] = model->normals[vn_idx[i]];
	}

	//�����淨��
	e1 = v[1] - v[0];
	e2 = v[2] - v[0];

	fn = e1.cross(e2);
	fn.normalize();

	//��������Ƭ�İ�Χ��
	bbox.min_bounding.x = min(model->vertexs[v_idx[0]].x, min(model->vertexs[v_idx[1]].x, model->vertexs[v_idx[2]].x));
	bbox.min_bounding.y = min(model->vertexs[v_idx[0]].y, min(model->vertexs[v_idx[1]].y, model->vertexs[v_idx[2]].y));
	bbox.min_bounding.z = min(model->vertexs[v_idx[0]].z, min(model->vertexs[v_idx[1]].z, model->vertexs[v_idx[2]].z));

	bbox.max_bounding.x = max(model->vertexs[v_idx[0]].x, max(model->vertexs[v_idx[1]].x, model->vertexs[v_idx[2]].x));
	bbox.max_bounding.y = max(model->vertexs[v_idx[0]].y, max(model->vertexs[v_idx[1]].y, model->vertexs[v_idx[2]].y));
	bbox.max_bounding.z = max(model->vertexs[v_idx[0]].z, max(model->vertexs[v_idx[1]].z, model->vertexs[v_idx[2]].z));

	//��Χ�е�����
	center_p = Vec3f(bbox.max_bounding + bbox.min_bounding) / 2;
}

Triangle::~Triangle()
{

}

//��������Ƭ��ĳ��ķ���
Vec3f Triangle::get_inter_point_normal(Vec3f point)
{

	//������Ƭ���������ĳ�㷨��
	Vec3f e3 = point - v[0];
	float e1e1 = e1.dot(e1);
	float e1e2 = e1.dot(e2);
	float e2e2 = e2.dot(e2);
	float e3e1 = e3.dot(e1);
	float e3e2 = e3.dot(e2);
	float diff = e1e1 * e2e2 - e1e2 * e1e2;
	float bv = (e2e2 * e3e1 - e1e2 * e3e2) / diff;
	float bw = (e1e1 * e3e2 - e1e2 * e3e1) / diff;
	float bu = 1 - bv - bw;

	Vec3f vn0bu = vn[0] * bu;
	Vec3f vn1bv = vn[1] * bv;
	Vec3f vn2bw = vn[2] * bw;

	Vec3f p_n = Vec3f(vn0bu.x + vn1bv.x + vn2bw.x, vn0bu.y + vn1bv.y + vn2bw.y, vn0bu.z + vn1bv.z + vn2bw.z);
	p_n.normalize();
	return p_n;

}

//������Ƭ����ߵ���
bool Triangle::tri_intersect(Ray& ray, Intersection& intersection)
{
    //�ο������������������ε��ཻ����㷨
	Vec3f P, T, Q;
	float det, t, b, c;  //������Ƭ�е����꣨a,b,c�� ,a = 1-b-c 

	if (fn == Vec3f(0.0f, 0.0f, 0.0f))
		return false;
	float tmp = fn.dot(ray.direction);
	if (fabs(tmp) < 1e-5)    //ray��������Ƭƽ�У����ཻ
		return false;
	P = ray.direction.cross(e2);
	T = v[0] - ray.origin;
	Q = ray.direction.cross(e1);
	//det = e1.dot(P);

	b = -T.dot(P) / e1.dot(P);
	c = -T.dot(Q) / e2.dot(Q);
	t = fn.dot(T) / tmp;

	if (b > 0 && c > 0 && b + c < 1 && t < ray.ray_max && t > ray.ray_min)
	{
		intersection.point = ray.origin + ray.direction * t;
		intersection.normal = get_inter_point_normal(intersection.point);
		intersection.material = t_material;
		ray.ray_max = t;
		return true;
	}
	return false;
}

Model::Model()
{
	
}

Model::~Model()
{
	for (int i = 0, len = triangles.size(); i < len; ++i)
	{
		if (triangles[i] != NULL)
		{
			delete triangles[i];
		}
	}
}

//�ָ��ַ���
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//���ض�Ӧ�Ĳ����ļ�
bool Model::LoadMaterial(string filename)
{
	ifstream infile(filename.c_str());
	if (!infile.is_open()) {
		cerr << "fail to open mtl" << filename << endl;
		return false;
	}

	bool flag = false;
	Material material;

	string str;

	while (infile >> str) {
		if (str == "newmtl") {
			if (flag) {
				materials.push_back(material);
				material = Material();
			}
			else {
				flag = true;
			}
			infile >> material.material_name;
		}
		else if (str == "Kd") {
			infile >> material.Kd.x >> material.Kd.y >> material.Kd.z;
		}
		else if (str == "Ka") {
			infile >> material.Ka.x >> material.Ka.y >> material.Ka.z;
		}
		else if (str == "Ks") {
			infile >> material.Ks.x >> material.Ks.y >> material.Ks.z;
		}
		else if (str == "Ke") {
			infile >> material.Ke.x >> material.Ke.y >> material.Ke.z;
		}
		else if (str == "Ns") {
			infile >> material.Ns;
		}
		else if (str == "Ni") {
			infile >> material.Ni;
		}
		else if (str == "Tf") {
			infile >> material.Tf.x >> material.Tf.y >> material.Tf.z;
		}
		else {
			infile.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}
	}

	if (flag) {
		materials.push_back(material);
	}
	infile.close();
}

//����ģ��
bool Model::LoadModel(string filename)
{
	ifstream infile(filename.c_str());
	if (!infile.is_open())
	{
		cout << "*********************** can't load the model ***********************" << endl <<endl;
		return false;
	}
	string str;

	int v_idx[3];
	int vn_idx[3];

	Material material;
	string m_name;
	cout << "*********************** begin load obj model ***********************" << endl;
	cout << "load obj name : " << filename << endl;

    while (infile >> str)
    {
        if (str == "mtllib")       //�����ļ���
        {
			infile >> m_name;
			LoadMaterial("model/" + m_name);  //���ض�Ӧ�Ĳ����ļ�
        }
		else if (str == "usemtl")              //��Ӧ��Ƭʹ�õĲ���
		{
			infile >> m_name;
			material = findMaterial(m_name);   //�ҵ���Ӧ�Ĳ���
		}
		else if (str == "v")                   //������Ϣ
		{
			Vec3f v_tmp;
			infile >> v_tmp.x >> v_tmp.y >> v_tmp.z;
			vertexs.push_back(v_tmp);

			//���Χ��
			if (v_tmp.x > bbox.max_bounding.x)
				bbox.max_bounding.x = v_tmp.x;
			if (v_tmp.y > bbox.max_bounding.y)
				bbox.max_bounding.y = v_tmp.y;
			if (v_tmp.z > bbox.max_bounding.z)
				bbox.max_bounding.z = v_tmp.z;
			if (v_tmp.x < bbox.min_bounding.x)
				bbox.min_bounding.x = v_tmp.x;
			if (v_tmp.y < bbox.min_bounding.y)
				bbox.min_bounding.y = v_tmp.y;
			if (v_tmp.z < bbox.min_bounding.z)
				bbox.min_bounding.z = v_tmp.z;

		}
		else if (str == "vn")                   //������Ϣ
		{
			Vec3f vn_tmp;
			infile >> vn_tmp.x >> vn_tmp.y >> vn_tmp.z;
			normals.push_back(vn_tmp);
		}
		else if (str == "f")                    //����Ϣ����������/��������/����������
		{
			int index = 0;
			memset(v_idx, -1, sizeof(v_idx));
			memset(vn_idx, -1, sizeof(vn_idx));

			while (true)
			{
				int t;
				char ch = infile.get();
				if (ch == ' ')
					continue;
				else if (ch == '\n' || ch == EOF)
					break;
				else
					infile.putback(ch);
				string tmp;
				infile >> tmp;
				//if (tmp == " ")            //�ո�����
				//	continue;
				//if (tmp == "f" || tmp == "usemtl" || tmp == "g" || tmp == "s" || tmp == "v" || tmp == "vt" || tmp == "vn")            //������һ������Ƭ�ˣ����ǰ��������ݶ�����Ż�ȥ����ֹ�˴�ѭ��
				//{
				//	tttt++;
				//	
				//	for(int i = tmp.size() - 1;i >= 0;i--)
				//		infile.putback(tmp[i]);
				//	break;
				//}
				vector<string> tmp_split;     //����'/'�ָ��ַ���
				SplitString(tmp, tmp_split, "/");
				int idx;
				if (tmp_split.size() == 2)
				{
					cout << "the f only has v and vn!!!!!!!!!!!!please check the code" << endl;
					v_idx[index] = atoi(tmp_split[0].c_str()) - 1;   //��������һ���ڵ�һ��
					vn_idx[index] = atoi(tmp_split[1].c_str()) - 1;  //��������һ���ڵڶ���
				}
				else
				{
					v_idx[index] = atoi(tmp_split[0].c_str()) - 1;   //��������һ���ڵ�һ��
					vn_idx[index] = atoi(tmp_split[2].c_str()) - 1;  //��������һ���ڵ�����
				}
					//��������һ���ڵڶ���������
				
				++index;
				if (index >= 3)//������Σ��ֽ��������
				{
					Triangle *triangle = new Triangle(v_idx, vn_idx, this);  //�洢������Ƭ����Ƭ����
					triangle->t_material = material;        //����Ƭ��Ӧ�Ĳ�����һ����
					triangles.push_back(triangle);
					//cout << "1111" << endl;

					//���һ��������Ƭ�󣬽��䶥��������ǰ�����Ա�����ӽ����Ķ��������ܺ�ǰ����������������Ƭ
					v_idx[1] = v_idx[2];
					vn_idx[1] = vn_idx[2];
					index = 2;

					
				}
			}
		}
		//cout << tttt << endl;
	}
	infile.close();
	kdtree = new KdTree();
	kdtree->build_kdtree(triangles);
	cout << "*********************** model load successfully ***********************" << endl << endl;
    return true;
}

//�Ӳ��ʱ����ҵ���Ӧ���ƵĲ���
Material Model::findMaterial(string name)
{
	Material f;
	for (auto m : materials)
		if (m.material_name == name)
			return m;
	cout << "can't find the material" << endl;
	return f;
}





