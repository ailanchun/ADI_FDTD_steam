//writen by liuzhichao 
//2016/10/8
// i-1/2 ---> i-1;    i+1/2---> i;   i+1---> i+1�������Ķ�Ӧ��ʽ
#ifndef _DEFINER_
#define _DEFINER_

#include<cstdio>
#include <iostream>
#include <stdlib.h>
#include <fstream> 
#include <string>
#include <math.h>
#include <time.h>
#include<windows.h>
using namespace std;
//Ҫ��ʹ��TE10ģʽ��Ϊǿ���Լ���Դ

const double X = 0.1; //��λm���������ȣ��������õĽϳ��Ա�۲�����
const double Y = 0.02286; //��λm��������ȣ�����BJ100��׼���
const double Z = 0.01016; //��λm, �����߶ȣ�����BJ100��׼�߶�

const int Nx = 501; //���ȷ���������
const int Ny = 201;//��ȷ���������
const int Nz = 101;//�߶ȷ���������

const double  dx = 2e-4; //l����ռ䲽��
const double dy = 1.143e-4; //w����ռ䲽��
const double dz = 1.1016e-4; //h����ռ䲽��

const double dt = 1.0e-14; // ʱ�䲽��
const int STEPS = 12000; //����6000��ʱ�䲽��������

const double freq = 9e9; //���䳡Ƶ��
const double pi = 3.1415926;
const double c = 2.99792458e8;
const double omega = 2 * pi*freq;//���ٶ�//

const double epsl0 = 8.854e-12;//���ɿռ��糣��
const double mur0 = 4.0*pi*1.0e-7;//���ɿռ䵼����
const double  k0 = omega*sqrt(mur0*epsl0);//���ɿռ䲨��//
const double kc = pi / Y; //TE10ģ��ֹ����//


const double epsl_x = (epsl0 - (dt / 2) * (dt / 2) * ((1 / mur0) / (dy * dy)));
const double epsl_y = (epsl0 - (dt / 2) * (dt / 2) * ((1 / mur0) / (dz * dz)));
const double epsl_z = (epsl0 - (dt / 2) * (dt / 2) * ((1 / mur0) / (dx * dx)));

const double mur_x = (mur0 - (dt / 2) * (dt / 2) * ((1 / epsl0) / (dy * dy)));
const double mur_y = (mur0 - (dt / 2) * (dt / 2) * ((1 / epsl0) / (dz * dz)));
const double mur_z = (mur0 - (dt / 2) * (dt / 2) * ((1 / epsl0) / (dx * dx)));



#endif