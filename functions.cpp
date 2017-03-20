#include"functions.h"
#include "definer.h"
#include "grid.h"

//============================��ʼ������==========================//
void initGrid(Grid* halfgrid_beforeX2, Grid* halfgrid_before, Grid* halfgrid_now)//����ĳ�ʼ��
{
	for (int k = 0; k<Nz; k++)
		for (int i = 0; i<Nx; i++)
			for (int j = 0; j<Ny; j++)
			{
				halfgrid_now[k*Nx*Ny + i*Ny + j].ex = 0.0;
				halfgrid_now[k*Nx*Ny + i*Ny + j].ey = 0.0;
				halfgrid_now[k*Nx*Ny + i*Ny + j].ez = 0.0;
				halfgrid_now[k*Nx*Ny + i*Ny + j].bx = 0.0;
				halfgrid_now[k*Nx*Ny + i*Ny + j].by = 0.0;
				halfgrid_now[k*Nx*Ny + i*Ny + j].bz = 0.0;

				halfgrid_before[k*Nx*Ny + i*Ny + j].ex = 0.0;
				halfgrid_before[k*Nx*Ny + i*Ny + j].ey = 0.0;
				halfgrid_before[k*Nx*Ny + i*Ny + j].ez = 0.0;
				halfgrid_before[k*Nx*Ny + i*Ny + j].bx = 0.0;
				halfgrid_before[k*Nx*Ny + i*Ny + j].by = 0.0;
				halfgrid_before[k*Nx*Ny + i*Ny + j].bz = 0.0;

				/*halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].ex = 0.0;
				halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].ey = 0.0;
				halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].ez = 0.0;
				halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].bx = 0.0;
				halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].by = 0.0;
				halfgrid_beforeX2[k*Nx*Ny + i*Ny + j].bz = 0.0;*/

			}

}

//==========================��ʼ������Դ==========================//
void init_source(Grid* halfgrid_now)//��ʼ������Դ//
{
	int k = 0;//�ӵ�Դ��λ��//
	int step = 0;
	for (int i = 0; i < Nx - 1; i++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			//��һ��������TE10,ģʽ

			//�糡
			double rising_edge = (step*dt) / (2 * T);

			/*halfgrid_now[i*Ny + j].ez = rising_edge * 0.0;*/

			halfgrid_now[i*Ny + j].ex = 0.0;

			double temp_ey = sin((pi / X)*i*dx);
			halfgrid_now[i*Ny + j].ey = rising_edge * temp_ey * sin(omega*step*dt);

			//�ų�
			/*double temp_bz = hm*cos((pi / X)*i*dx);
			halfgrid_now[i*Ny + j].bz = rising_edge * temp_bz * cos(omega*step*dt);*/

			/*double temp_bx = (X*bate / pi)*hm*sin((pi / X)*i*dx);*/

			/*halfgrid_now[i*Ny + j].bx = -1 * rising_edge * temp_bx * sin(omega*step*dt*1.5);*/

			/*halfgrid_now[i*Ny + j].bx = rising_edge * 0.0;*/

			/*halfgrid_now[i*Ny + j].by = rising_edge * 0.0;*/

		}
	}
}

//=============================������===========================//
void save_result(ofstream file, Grid* halfgrid_now, int step)
{
	int result_z = 2;
	int result_x = 10;
	int result_y = 5;

	file << step << '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ex
		<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ey
		<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ez << '\t';

	file << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].bx
		<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].by
		<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].bz << '\t';

	file << '\n';

}

//=========================������������=========================//
void get_plat(ofstream file2, Grid* halfgrid_now)//��ȡһ�����������
{
	for (int i = 0; i < Nx - 1; i++)
	{
		for (int k = 0; k < Nz - 1; k++)
		{
			file2 << k << '\t' << i << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].ey << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].bx << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].bz << endl;
		}
	}

}

//============================�ͷſռ�===========================//
void free(Grid* halfgrid_beforeX2, Grid* halfgrid_before, Grid* halfgrid_now)
{
	delete halfgrid_before;
	delete halfgrid_now;
	delete halfgrid_beforeX2;
	//delete grid_result;
}

//=======================����CFL�ȶ�������=======================//
double CFL_calc()
{
	double result = 0.0;
	double v = 1 / (sqrt(epsl0*mur0));
	double g_temp = sqrt((1 / dx)*(1 / dx) + (1 / dy)*(1 / dy) + (1 / dz)*(1 / dz));
	result = 1 / (v*g_temp);
	return result;
}
double fc_calc()
{
	double fc = 1 / (2 * X * sqrt(epsl0*mur0));
	return fc;
}

//===========================�������ֵ=========================//
int theor_val_gen()
{
	clock_t start_time = clock();
	system("mkdir result\\theor_val");
	double ex = 0.0;
	double ey = 0.0;
	double ez = 0.0;
	double hx = 0.0;
	double hy = 0.0;
	double hz = 0.0;
	double temp0 = 0.0;
	double temp1 = 0.0;
	double temp2 = 0.0;

	int step = 5;

	int result_z = 10;
	int result_x = 10;
	int result_y = 5;//10,10,5

	ofstream file(theor_val_filepath);//���ڱ�����

	while (step < STEPS)
	{
		if (step*dt < 2 * T)
		{
			//=============================�糡=============================//
			ex = ((step*dt) / (2 * T))*0.0;//ex

										   //double temp0 = ((omega*mur0*X) / pi) * hm * sin((pi / X)*result_x*dx);
			double temp0 = hm * sin((pi / X)*result_x*dx);
			double temp_ey_sin = omega*step*dt - bate*result_z;
			double temp_ey_font = (step*dt) / (2 * T);

			ey = temp_ey_font * temp0 * sin(temp_ey_sin);//ey


			ez = ((step*dt) / (2 * T))*0.0;//ez

										   //=============================�ų�=============================//

										   //double temp1 = (X*bate / pi) * sin((pi / X) * result_x * dx);
			double temp1 = (bate / (omega*mur0))*sin((pi / X) * result_x * dx);
			double temp_hx_sin = omega*step*dt - bate*result_z;
			double temp_hx_font = -1 * (step*dt) / (2 * T) * hm;

			hx = temp_hx_font * temp1 * sin(temp_hx_sin);//hx

			hy = ((step*dt) / (2 * T))*0.0;//hy

			double temp4 = (pi / X) * result_x*dx;
			double temp2 = (pi / (omega*mur0*X))*hm * cos(temp4);
			double temp3 = omega*step*dt - bate*result_z;
			double temp_hz_font = (step*dt) / (2 * T);

			hz = temp_hz_font * temp2 * cos(temp3);//hz
		}
		else
		{
			//=============================�糡=============================//
			ex = 0.0;//ex
			double temp_ey_sin_1 = (pi / X)*result_x*dx;
			//double temp0 = ((omega*mur0*X) / pi) * hm * sin(temp_ey_sin_1);
			double temp0 = hm * sin(temp_ey_sin_1);
			double temp_ey_sin_2 = omega*step*dt - bate*result_z;
			ey = temp0*sin(temp_ey_sin_2);//ey
			ez = 0.0;//ez
					 //=============================�ų�=============================//
			double temp_hx_1 = (pi / X) * result_x * dx;
			//double temp1 = (X*bate / pi)* hm* sin(temp_hx_1);
			double temp1 = (bate / (omega*mur0))* hm* sin(temp_hx_1);
			double temp_hx_2 = omega*step*dt - bate*result_z;
			hx = -1 * temp1 * sin(temp_hx_2);//hx
											 //����bate���ڲ�ͬ��Ƶ���²�����������൱������������Դ
			hy = 0.0;//hy
			double temp2 = (pi / (omega*mur0*X))* hm * cos((pi / X) * result_x*dx);
			double temp_hz_cos = omega*step*dt - bate*result_z;
			hz = temp2 * cos(temp_hz_cos);//hz
		}
		file << step << '\t' << ex << '\t' << ey << '\t' << ez << '\t' << hx << '\t' << hy << '\t' << hz << endl;
		cout << "STEP---" << step << "  has finished" << endl;
		step++;
	}

	clock_t end_time = clock();
	cout << "����ֵ��������ʱ�� : " << static_cast<float>(end_time - start_time) / CLOCKS_PER_SEC << " s\n" << endl;
	return 0;
}
