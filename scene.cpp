
#include "glut.h"	
#include <stdio.h> 	
#include "glaux.h"
#include <math.h>			// 数学库


float xpos;								//	x方向坐标
float zpos;								//	z方向坐标
GLfloat	z = 0.0f;							//  头部上下旋转

typedef struct tagVERTEX// 创建Vertex顶点结构
{
	float x, y, z;   // 3D 坐标
	float u, v;		// 纹理坐标
} VERTEX;

typedef struct tagTRIANGLE//一个sector(区段)包含了一系列的多边形，所以我就定义了一个triangle(我的墙体、地板和天花板用三角形构成）
{
	VERTEX vertex[3];  // VERTEX矢量数组，大小为3
} TRIANGLE;

typedef struct tagSECTOR//每个3D世界基本上可以看作是sector(区段)的集合。一个sector可以是一个房间、一个立方体、或者任意一个闭合的区间
{
	int numtriangles;  // Sector中的三角形个数
	TRIANGLE* triangle;// 指向三角数组的指针
} SECTOR;



SECTOR sector1;				//创建一个世界

GLfloat		xrot;			// X 旋转量
GLfloat		yrot;			// Y 旋转量

BOOL	light;				// 光源的开/关
BOOL	lp;					// L键按下了么?
bool    blend;				// 是否混合?



GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f }; 	// 环境光参数
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// 漫射光参数
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };	// 光源位置

GLuint	texture[4];										// 4种纹理的储存空间




//---------------读入字符串函数---------------------

void readstr(FILE *f, char *string)							//  读入一个字符串

{
	do														// 循环开始
	{
		fgets(string, 255, f);								// 读入一行
	} while ((string[0] == '/') || (string[0] == '\n'));	// 考察是否有必要进行处理
	return;													// 返回
}

//-----------------------设置世界函数-------------------------

void SetupWorld()											// 设置我们的世界
{
	FILE *filein;											// 工作文件
	filein = fopen("data/world.txt", "rt");					// 打开文件

	int numtriangles;										// 区段中的三角形数量
	char oneline[255];										// 存储数据的字符串
	float x, y, z, u, v;									// 3D 和 纹理坐标

	readstr(filein, oneline);								// 读入一行数据
	sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);		// 读入三角形数量


	sector1.triangle = new TRIANGLE[numtriangles];			// 为numtriangles个三角形分配内存并设定指针
	sector1.numtriangles = numtriangles;					// 定义区段1中的三角形数量
	// 遍历区段中的每个三角形
	for (int triloop = 0; triloop < numtriangles; triloop++)// 遍历所有的三角形
	{
		// 遍历三角形的每个顶点
		for (int vertloop = 0; vertloop < 3; vertloop++)	// 遍历所有的顶点
		{
			readstr(filein, oneline);						// 读入一行数据
			// 读入各自的顶点数据
			sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
			// 将顶点数据存入各自的顶点
			sector1.triangle[triloop].vertex[vertloop].x = x;	// 区段 1,  第 triloop 个三角形, 第  vertloop 个顶点, 值 x =x
			sector1.triangle[triloop].vertex[vertloop].y = y;	// 区段 1,  第 triloop 个三角形, 第  vertloop 个顶点, 值 y =y
			sector1.triangle[triloop].vertex[vertloop].z = z;	// 区段 1,  第 triloop 个三角形, 第  vertloop 个顶点, 值  z =z
			sector1.triangle[triloop].vertex[vertloop].u = u;	// 区段 1,  第 triloop 个三角形, 第  vertloop 个顶点, 值  u =u
			sector1.triangle[triloop].vertex[vertloop].v = v;	// 区段 1,  第 triloop 个三角形, 第  vertloop 个顶点, 值  e=v
		}
	}
	fclose(filein);
	return;													// 返回
}



AUX_RGBImageRec *LoadBMP(CHAR *Filename)				// 载入位图图象
{
	FILE *File = NULL;									// 文件句柄
	if (!Filename)										// 确保文件名已提供
	{
		return NULL;									// 如果没提供，返回 NULL
	}
	File = fopen(Filename, "r");							// 尝试打开文件
	if (File)											// 文件存在么?
	{
		fclose(File);									// 关闭句柄
		return auxDIBImageLoadA(Filename);				// 载入位图并返回指针
	}
	return NULL;										// 如果载入失败，返回 NULL
}

int LoadGLTextures()									// 载入位图(调用上面的代码)并转换成纹理
{
	int Status = FALSE;									// 状态指示器
	AUX_RGBImageRec *TextureImage[4];					// 创建纹理的存储空间
	memset(TextureImage, 0, sizeof(void *)* 1);			// 将指针设为 NULL

	//------------------------载入纹理图片------------------------//

	if (TextureImage[0] = LoadBMP("Data/ceiling.bmp"))
	{
		Status = TRUE;									// 状态设为 TRUE
		glGenTextures(1, &texture[0]);					// 创建纹理
		// 创建线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[1] = LoadBMP("Data/door.bmp"))
	{
		Status = TRUE;									// 状态设为 TRUE
		glGenTextures(1, &texture[1]);					// 创建纹理
		// 创建线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);
	}
	if (TextureImage[2] = LoadBMP("Data/floor.bmp"))
	{
		Status = TRUE;									// 状态设为 TRUE
		glGenTextures(1, &texture[2]);					// 创建纹理
		// 创建线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[2]->sizeX, TextureImage[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[2]->data);
	}

	if (TextureImage[3] = LoadBMP("Data/wall.bmp"))
	{
		Status = TRUE;									// 状态设为 TRUE
		glGenTextures(1, &texture[3]);					// 创建纹理
		// 创建线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[3]->sizeX, TextureImage[3]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[3]->data);
	}

	for (char h = 3; h>0; h--){
		if (TextureImage[h])								// 纹理是否存在
		{
			if (TextureImage[h]->data)						// 纹理图像是否存在
			{
				free(TextureImage[h]->data);				// 释放纹理图像占用的内存
			}

			free(TextureImage[h]);							// 释放图像结构
		}
	}
	return Status;										// 返回 Status
}




GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// 重置OpenGL窗口大小
{
	if (height == 0)										// 防止被零除
	{
		height = 1;										// 将Height设为1
	}

	glViewport(0, 0, width, height);						// 重置当前的视口

	glMatrixMode(GL_PROJECTION);						// 选择投影矩阵
	glLoadIdentity();									// 重置投影矩阵

	// 设置视口的大小
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// 选择模型观察矩阵
	glLoadIdentity();									// 重置模型观察矩阵
}



int InitGL(GLvoid)										// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())								// 调用纹理载入子例程
	{
		return FALSE;									// 如果未能载入，返回FALSE
	}

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// 设置环境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// 设置漫射光
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);	// 设置光源位置
	glEnable(GL_LIGHT1);								// 启用一号光源
	glEnable(GL_TEXTURE_2D);							// 启用纹理映射
	glShadeModel(GL_SMOOTH);							// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// 黑色背景
	glClearDepth(1.0f);									// 设置深度缓存
	glEnable(GL_DEPTH_TEST);							// 启用深度测试
	glDepthFunc(GL_LEQUAL);								// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// 告诉系统对透视进行修正
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);						// 全亮度， 50% Alpha 混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// 基于源象素alpha通道值的半透明混合函数

	SetupWorld();
	return TRUE;										// 初始化 OK
}

int DrawGLScene(GLvoid)												// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// 清除屏幕和深度缓存
	glLoadIdentity();												// 重置当前的模型观察矩阵

	GLfloat x_m, y_m, z_m, u_m, v_m;								// 顶点的临时 X, Y, Z, U 和 V 的数值
	GLfloat xtrans = -xpos;											// 用于游戏者沿X轴平移时的大小
	GLfloat ztrans = -zpos;											// 用于游戏者沿Z轴平移时的大小
	GLfloat ytrans = -0.7;											// 用于设置游戏者头部高度
	GLfloat sceneroty = 360.0f - yrot;								// 位于游戏者方向的360度角

	int numtriangles;												// 保有三角形数量的整数

	glRotatef(z, 1.0f, 0, 0);											// 上下旋转
	glRotatef(sceneroty, 0, 1.0f, 0);									// 根据游戏者正面所对方向所作的旋转

	glTranslatef(xtrans, ytrans, ztrans);							// 以游戏者为中心的平移场景
	numtriangles = sector1.numtriangles;							// 取得Sector1的三角形数量

	// 逐个处理三角形
	for (int loop_m = 0; loop_m < numtriangles; loop_m++)			// 遍历所有的三角形
	{
		if (loop_m == 0 || loop_m == 1) glBindTexture(GL_TEXTURE_2D, texture[2]);    //根据墙壁的位置绑定纹理
		if (loop_m == 2 || loop_m == 3) glBindTexture(GL_TEXTURE_2D, texture[0]);
		if (loop_m == 4 || loop_m == 5) glBindTexture(GL_TEXTURE_2D, texture[3]);
		if (loop_m == 6 || loop_m == 7) glBindTexture(GL_TEXTURE_2D, texture[4]);
		if (loop_m == 8 || loop_m == 9) glBindTexture(GL_TEXTURE_2D, texture[5]);
		if (loop_m == 10 || loop_m == 11) glBindTexture(GL_TEXTURE_2D, texture[5]);
		if (loop_m == 12 || loop_m == 13) glBindTexture(GL_TEXTURE_2D, texture[6]);
		if (loop_m == 14 || loop_m == 15) glBindTexture(GL_TEXTURE_2D, texture[6]);
		if (loop_m == 16 || loop_m == 17) glBindTexture(GL_TEXTURE_2D, texture[6]);
		if (loop_m == 18 || loop_m == 19) glBindTexture(GL_TEXTURE_2D, texture[1]);

		glBegin(GL_TRIANGLES);										// 开始绘制三角形
		glNormal3f(0.0f, 0.0f, 1.0f);							// 指向前面的法线
		x_m = sector1.triangle[loop_m].vertex[0].x;				// 第一点的 X 分量
		y_m = sector1.triangle[loop_m].vertex[0].y;				// 第一点的 Y 分量
		z_m = sector1.triangle[loop_m].vertex[0].z;				// 第一点的 Z 分量
		u_m = sector1.triangle[loop_m].vertex[0].u;				// 第一点的 U  纹理坐标
		v_m = sector1.triangle[loop_m].vertex[0].v;				// 第一点的 V  纹理坐标
		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);			// 设置纹理坐标和顶点

		x_m = sector1.triangle[loop_m].vertex[1].x;				// 第二点的 X 分量
		y_m = sector1.triangle[loop_m].vertex[1].y;				// 第二点的 Y 分量
		z_m = sector1.triangle[loop_m].vertex[1].z;				// 第二点的 Z 分量
		u_m = sector1.triangle[loop_m].vertex[1].u;				// 第二点的 U  纹理坐标
		v_m = sector1.triangle[loop_m].vertex[1].v;				// 第二点的 V  纹理坐标
		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);			// 设置纹理坐标和顶点

		x_m = sector1.triangle[loop_m].vertex[2].x;				// 第三点的 X 分量
		y_m = sector1.triangle[loop_m].vertex[2].y;				// 第三点的 Y 分量
		z_m = sector1.triangle[loop_m].vertex[2].z;				// 第三点的 Z 分量
		u_m = sector1.triangle[loop_m].vertex[2].u;				// 第二点的 U  纹理坐标
		v_m = sector1.triangle[loop_m].vertex[2].v;				// 第二点的 V  纹理坐标
		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);			// 设置纹理坐标和顶点
		glEnd();// 三角形绘制结束
	}



	return TRUE;										// 一切 OK
}


