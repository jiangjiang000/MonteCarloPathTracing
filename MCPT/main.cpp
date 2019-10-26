#include <iostream>
#include "Model.h"
#include "MonteCarloPathTrace.h"
#include <gl/glut.h>
#include<gl/freeglut.h>
#include<gl/GL.h>
#include<gl/GLU.h>

using namespace std;


#define BMP_Header_Length 54

Model model;
MonteCarloPathTrace* pathTracer = NULL;
int width = 300, height = 300;
int index = 0;

//ץȡ�����е�����
void savepic(string filename)
{
	FILE* pDummyFile;
	FILE* pWritingFile;
	GLubyte* pPixelData;
	GLubyte BMP_Header[BMP_Header_Length];
	GLint i, j;
	GLint PixelDataLength;
	// �����������ݵ�ʵ�ʳ���
	i = width * 3; // �õ�ÿһ�е��������ݳ���
	while (i % 4 != 0) // �������ݣ�ֱ�� i�ǵı���
		++i; // �������и�����㷨��
	// �������׷��ֱ�ۣ����ٶ�û��̫��Ҫ��
	PixelDataLength = i * height;
	// �����ڴ�ʹ��ļ�
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);
	pDummyFile = fopen("test.bmp", "rb");//��һ����ȷ��bmp�ļ��ж�ȡǰ54���ֽڣ��޸����еĿ�Ⱥ͸߶���Ϣ���Ϳ��Եõ��µ��ļ�ͷ
	if (pDummyFile == 0)
		exit(0);

	pWritingFile = fopen(filename.c_str(), "wb");

	if (pWritingFile == 0)
		exit(0);
	// ��ȡ����
	// GL_UNPACK_ALIGNMENTָ��OPenGL��δ����ݻ������н��ͼ������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);
	// �� whole.bmp ���ļ�ͷ����Ϊ���ļ����ļ�ͷ
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = width;
	j = height;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);
	// �ͷ��ڴ�͹ر��ļ�
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}

void choose_model(int index)
{
	if (index == 0)
	{
		width = 600;
		height = 600;
		//objģ�ͼ���

		model.LoadModel("model/room.obj");

		//�������������
		pathTracer = new MonteCarloPathTrace(&model, width, height);
		pathTracer->setCamera(Vec3f(0, 0, 4), Vec3f(0, 0, 0), Vec3f(0, 1, 0), 50.0f, width, height);
		pathTracer->lights.push_back(Light(Vec3f(0.0, 1.589, -1.274), 0.2, Vec3f(50, 50, 40), "sphere", 2));

	}
	else if (index == 1)
	{
		width = 600;
		height = 600;
		//objģ�ͼ���
		//Model model;
		model.LoadModel("model/cup.obj");

		//�������������
		pathTracer = new MonteCarloPathTrace(&model, width, height);
		pathTracer->setCamera(Vec3f(0, 0.64, 0.52), Vec3f(0, 0.4, 0.3), Vec3f(0, 1.0, 0), 60, width, height);
		pathTracer->lights.push_back(Light(Vec3f(-2.758771896, 1.5246, 0.0), Vec3f(0, 1.5, 1.5), Vec3f(0, -1.5, 1.5), Vec3f(40, 40, 40), "quad"));

	}
	else if (index == 2)
	{
		width = 600;
		height = 450;
		//objģ�ͼ���
		//Model model;
		model.LoadModel("model/VeachMIS.obj");

		//�������������
		pathTracer = new MonteCarloPathTrace(&model, width, height);
		pathTracer->setCamera(Vec3f(0.0, 2.0, 15.0), Vec3f(0.0, 1.69521, 14.0476), Vec3f(0, 0.952421, -0.304787), 40, width, height);

		pathTracer->lights.push_back(Light(Vec3f(-10, 10, 4), 0.5, Vec3f(800, 800, 800), "sphere", 1.2));
		pathTracer->lights.push_back(Light(Vec3f(3.75, 0, 0), 0.033, Vec3f(901.803, 901.803, 901.803), "sphere", 1.2));
		pathTracer->lights.push_back(Light(Vec3f(1.25, 0, 0), 0.1, Vec3f(100, 100, 100), "sphere", 1.2));
		pathTracer->lights.push_back(Light(Vec3f(-1.25, 0, 0), 0.3, Vec3f(11.1111, 11.1111, 11.1111), "sphere", 1.2));
		pathTracer->lights.push_back(Light(Vec3f(-3.75, 0, 0), 0.9, Vec3f(1.23457, 1.23457, 1.23457), "sphere", 1.2));

	}
	else if (index == 3)
	{
		width = 600;
		height = 600;
		//objģ�ͼ���
		//Model model;
		model.LoadModel("model/venus.obj");

		//�������������
		pathTracer = new MonteCarloPathTrace(&model, width, height);
		pathTracer->setCamera(Vec3f(-1.2, 0.0, 2.00), Vec3f(0.0, 0.0, 0.0), Vec3f(0, 1.0, 0), 50, width, height);
		pathTracer->lights.push_back(Light(Vec3f(-2.058771896, 1.5246, 0.0), Vec3f(0, 1.5, 1.5), Vec3f(0, -1.5, 1.5), Vec3f(50, 50, 40), "quad"));

	}
	else
		cout << "choose model ERROR!!!" << endl;

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	float* colors = pathTracer->run();
	
	/*for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			cout << "(w,h)=" << w << "," << h << "=" << colors[h * width * 3 + w * 3] << endl;
		}
	}*/
	glRasterPos2i(0, 0);
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, (GLvoid *)colors);

	glutSwapBuffers();
	if (pathTracer->it_num % 10 == 1 && pathTracer->it_num <= 101)
		savepic("result/" + to_string(index) + "_"+ to_string(pathTracer->it_num - 1) + ".bmp");
}

void idle()
{
	glutPostRedisplay();
}

void init()
{
	cout << "********************* MONTE CARLO PATH TRACING ****************************" << endl;
	cout << "model list: 0.room.obj && room.mtl"<< endl; ;
	cout << "            1.cup.obj && cup.mtl" << endl;
	cout << "            2.Veach MIS.obj && Veach MIS.mtl" << endl;
	cout << "            3.venus.obj && venus.mtl" << endl;
	cout << "***************************************************************************" << endl;
	cout << "please choose one model to render(0~3) : " ;
	cin >> index;
	
	cout << endl;
}

int main(int argc, char *argv[])
{
	init();
	choose_model(index);
  
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Monte Carlo Path Tracing");
	glutDisplayFunc(display);

	glutIdleFunc(idle);

	glutMainLoop();

	//cout << "111" << endl;
	system("pause");
    return 0;
}