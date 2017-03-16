#ifndef _3D_FDTD_
#define _3D_FDTD_

#include "definer.h"
#include "grid.h"
#include"functions.h"
#include"Gss-2.0.h"
//1.3�������˹�ʽ�����Ⲣ����������������ѭ����Χ������
//1.4�����������FDTD���룬�������CFL�ȶ�������->���㡣�洢���̣�->���ָ�������Ҫhalfgrid-before.ֱ����now���¼���
//�߽紦��ѭ�����ƣ��������ã�
//-----------------------------------------------------------------------��������
void fdtd_bz(Grid* halfgrid_now, int step);
void fdtd_bx(Grid* halfgrid_now, int step);
void fdtd_by(Grid* halfgrid_now, int step);

void fdtd_ez(Grid* halfgrid_now, int step);
void fdtd_ex(Grid* halfgrid_now, int step);
void fdtd_ey(Grid* halfgrid_now, int step);

void fdtd_matel(Grid* halfgrid_now);
void fdtd_bz(Grid* halfgrid_now, int step);


#endif