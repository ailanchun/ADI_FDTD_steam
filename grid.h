//writen by liuzhichao 
//2016/10/8
#ifndef _GRID_
#define _GRID_
#include"definer.h"

class Grid	
{
public:
    double ex,ey,ez;//�糡ǿ��
	double bx,by,bz;//�ų�ǿ��
	
};
//����ͳһ��Ϊһά�����飬�����߲��л�
Grid* halfgrid_now = new  Grid[Nx*Ny*Nz]; //���������������ʱ��
Grid* halfgrid_before = new Grid[Nx*Ny*Nz];//������������ȥʱ��
Grid* halfgrid_beforeX2 = new  Grid[Nx*Ny*Nz];//������������ȥ�Ĺ�ȥʱ��,mur���ձ߽�Ҫ��
//Grid* grid_result = new Grid[Nx*Ny*Nz];//���������г��Ľ��

//Grid halfgrid_now[Ny*Nz*Nz]; //���������������ʱ��
//Grid halfgrid_before[Ny*Nz*Nz];//������������ȥʱ��
//Grid grid_result[Ny*Nz*Nz];//���������г����










#endif