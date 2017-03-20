#include"GSS_ADI_CPML.h"
//TODO:  �ܵ���============================//
void adi_fdtd_leapforg_cpml_GSS(Grid *halfgrid_now)
{

	//=========================����ļ�����=========================//
	ofstream file_matle(cpml_filepath);
	ofstream file_matle_plat(cpml_p_filepath);

	int step = 0;//ʱ�䲽��
	int result_z = 10;//10,10,5
	int result_x = 10;
	int result_y = 5;

	while (step < STEPS)
	{
		//����˼�룺�ֱ������������ȫ�����ֵ�����ڼ��㲻��Ҫ��ǰʱ�̵�ֵ�����Էֲ�ȫ�ּ���

		gss_cal_cpml(halfgrid_now, step);

		//==========================������==========================//
		/*file_matle << step << '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ey;*/

		file_matle << step << '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ex
			<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ey
			<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].ez << '\t';

		file_matle << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].bx
			<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].by
			<< '\t' << halfgrid_now[result_z * Nx*Ny + result_x * Ny + result_y].bz << '\t';

		file_matle << '\n';

		cout << "Step--- " << step << " ---has finished." << endl;
		step++;

	}//while
	file_matle.close();

	//=======================������������========================//
	for (int i = 0; i < Nx - 1; i++)
	{
		for (int k = 0; k < Nz - 1; k++)
		{
			file_matle_plat << k << '\t' << i << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].ey << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].bx << '\t'
				<< halfgrid_now[k*Nx*Ny + i*Ny + 5].bz << endl;
		}
	}

	file_matle_plat.close();

}//������β

 //TODO:  gss�������=========================//
void gss_cal_cpml(Grid* halfgrid_now, int step)
{
	//-------------------------------------------------------------------����糡

	//����˼�룺�ֱ������������ȫ�����ֵ�����ڼ��㲻��Ҫ��ǰʱ�̵�ֵ�����Էֲ�ȫ�ּ���

	norm_gsscalc_ez(halfgrid_now, step);
	cpml_gsscalc_ez(halfgrid_now, step);

	norm_gsscalc_ex(halfgrid_now, step);
	cpml_gsscalc_ex(halfgrid_now, step);

	norm_gsscalc_ey(halfgrid_now, step);
	cpml_gsscalc_ey(halfgrid_now, step);

	//-------------------------------------------------------------------����ų�
	norm_gsscalc_bz(halfgrid_now, step);
	cpml_gsscalc_bz(halfgrid_now, step);

	norm_gsscalc_bx(halfgrid_now, step);
	cpml_gsscalc_bx(halfgrid_now, step);

	norm_gsscalc_by(halfgrid_now, step);
	cpml_gsscalc_by(halfgrid_now, step);

}

//TODO:  gss����ʵ��//
void norm_gsscalc_ez(Grid* halfgrid_now, int step)
{
	double aez = (-1 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dx)*(1 / dx));
	double bez = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dx)*(1 / dx));
	double cez = aez;

	int nRet;//GSS�����ķ���ֵ
	int N = Nx - 1;

	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nx];
	int ind[3 * (Nx - 1) - 2];
	double val[3 * (Nx - 1) - 2];
	double rhs[Nx - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}

	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bez;
	val[2] = cez;
	val[3 * N - 3] = bez;
	val[3 * N - 5] = aez;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = aez;
		val[i + 2] = bez;
		val[i + 4] = cez;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;
	//����ϵ������

	for (int k = 0; k < s0; k++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int i = 0; i < Nx - 1; i++)
			{
				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (i == 0 && j != 0)
					rhs[i] = bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by) / dx - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx) / dy);
				else if (j == 0 && i != 0)
					rhs[i] = aez*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].ez + bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].by) / dx - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / dy);
				else if (i == 0 && j == 0)
					rhs[i] = bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by) / dx - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / dy);
				else if (i != 0 && j != 0)
					rhs[i] = aez*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].ez + bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].by) / dx - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx) / dy);
			}


			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int i1 = 0; i1 < Nx - 1; i1++)
			{
				//�Խ����������								
				if (j == 0 || j == Ny - 2 || i1 == 0 || i1 == Nx - 2)//����ez�ı߽����⣬���ĸ����λ��Ӧ��Ϊ0
				{
					rhs[i1] = 0.0;
				}
				//������	
				halfgrid_now[k*Nx*Ny + i1*Ny + j].ez = rhs[i1];
			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);

		}
	}
}

void norm_gsscalc_ex(Grid* halfgrid_now, int step)
{
	double aex = (-1 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dy)*(1 / dy));
	double bex = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dy)*(1 / dy));
	double cex = aex;

	int nRet;//GSS�����ķ���ֵ
	int N = Ny - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Ny];
	int ind[3 * (Ny - 1) - 2];
	double val[3 * (Ny - 1) - 2];
	double rhs[Ny - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bex;
	val[2] = cex;
	val[3 * N - 3] = bex;
	val[3 * N - 5] = aex;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = aex;
		val[i + 2] = bex;
		val[i + 4] = cex;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int k = 0; k < Nz - 1; k++)
	{
		for (int i = 0; i < Nx - 1; i++)
		{
			for (int j = 0; j < Ny - 1; j++)
			{
				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (j == 0 && k != 0)
					rhs[j] = bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / dy - (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by) / dz);
				else if (k == 0 && j != 0)
					rhs[j] = aex*halfgrid_now[k*Nx*Ny + i*Ny + j - 1].ex + bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bz) / dy - (halfgrid_now[k*Nx*Ny + i*Ny + j].by) / dz);
				else if (j == 0 && k == 0)
					rhs[j] = bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / dy - (halfgrid_now[k*Nx*Ny + i*Ny + j].by) / dz);
				else if (j != 0 && k != 0)
					rhs[j] = aex*halfgrid_now[k*Nx*Ny + i*Ny + j - 1].ex + bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bz) / dy - (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by) / dz);
			}//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int j1 = 0; j1 < Ny - 1; j1++)
			{
				//�Խ����������				
				if (j1 == 0 || j1 == Ny - 2 || k == Nz - 2 || k == 0)
				{
					rhs[j1] = 0.0;
				}
				halfgrid_now[k*Nx*Ny + i*Ny + j1].ex = rhs[j1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}

	}
}

void norm_gsscalc_ey(Grid* halfgrid_now, int step)
{
	double aey = (-1 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dz)*(1 / dz));
	double bey = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / dz)*(1 / dz));
	double cey = aey;

	int nRet;//GSS�����ķ���ֵ
	int N = Nz - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nz];
	int ind[3 * (Nz - 1) - 2];
	double val[3 * (Nz - 1) - 2];
	double rhs[Nz - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}

	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bey;
	val[2] = cey;
	val[3 * N - 3] = bey;
	val[3 * N - 5] = aey;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = aey;
		val[i + 2] = bey;
		val[i + 4] = cey;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int i = 0; i < Nx - 1; i++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int k = 0; k < Nz - 1; k++)
			{
				if (k == 0 && i != 0)
					rhs[k] = bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz) / dx);
				else if (i == 0 && k != 0)
					rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / dx);
				else if (k == 0 && i == 0)
					rhs[k] = bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / dx);
				else if (k != 0 && i != 0)
					rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz) / dx);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);
			//����������뱣��
			for (int k1 = 0; k1 < Nz - 1; k1++)
			{
				if (k1 == 0) //�Խ����������
				{//���ı�Դ��ֵ					
					if (step*dt < 2 * T)
					{
						double rising_edge = (step*dt) / (2 * T);
						double temp_ey = hm * sin((pi / X)*i*dx);

						rhs[k1] = rising_edge * temp_ey * sin(omega*step*dt);//ey
					}
					else
					{
						double temp_ey = hm * sin((pi / X)*i*dx);

						rhs[k1] = temp_ey*sin(omega*step*dt);//ey
					}
					//double temp_ey = sin((pi / X)*i*dx);
					// 
					//rhs[k1] = temp_ey*sin(omega*step*dt);//ey
				}

				if (i == 0 || i == Nx - 2 || k1 == Nz - 2)
				{
					rhs[k1] = 0.0;
				}
				//������
				halfgrid_now[k1*Nx*Ny + i*Ny + j].ey = rhs[k1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}
}

void norm_gsscalc_bz(Grid* halfgrid_now, int step)
{
	double ahz = (-1 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dx)*(1 / dx));
	double bhz = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dx)*(1 / dx));
	double chz = ahz;

	int nRet;//GSS�����ķ���ֵ
	int N = Nx - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nx];
	int ind[3 * (Nx - 1) - 2];
	double val[3 * (Nx - 1) - 2];
	double rhs[Nx - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bhz;
	val[2] = chz;
	val[3 * N - 3] = bhz;
	val[3 * N - 5] = ahz;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = ahz;
		val[i + 2] = bhz;
		val[i + 4] = chz;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int k = 0; k < Nz - 1; k++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int i = 0; i < Nx - 1; i++)
			{
				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (i == 0)
					rhs[i] = bhz*halfgrid_now[k*Nx*Ny + i*Ny + j].bz + chz*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].bz
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / dy - (halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / dx);
				else
					rhs[i] = ahz*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz + bhz*halfgrid_now[k*Nx*Ny + i*Ny + j].bz + chz*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].bz
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / dy - (halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / dx);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;	//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);
			for (int i1 = 0; i1 < Nx - 1; i1++)
			{
				//�Խ����������				
				//if (k == 0)//���ı�����Դ��ֵ
				//{
				//	double rising_edge = (step*dt) / (2 * T);

				//	if (step*dt < 2 * T)
				//	{
				//		double temp_bz = hm * cos((pi / X) * i1*dx);
				//	
				//		rhs[i1]= rising_edge * temp_bz * cos(omega*step*dt);//hz
				//	}
				//	else
				//	{
				//		double temp_bz = hm * cos((pi / X) * i1 * dx);

				//		rhs[i1] =  temp_bz * cos(omega * step * dt);//hz
				//	}
				//	
				//}
				if (k == Nz - 2)//���ҽ����Դ����ʹ�丳ֵΪ0�������൱���������ձ߽�
				{
					rhs[i1] = 0.0;
				}
				//������
				halfgrid_now[k*Nx*Ny + i1*Ny + j].bz = rhs[i1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}

}

void norm_gsscalc_bx(Grid* halfgrid_now, int step)
{
	double ahx = (-1 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dy)*(1 / dy));
	double bhx = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dy)*(1 / dy));
	double chx = ahx;

	int nRet;//GSS�����ķ���ֵ
	int N = Ny - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Ny];
	int ind[3 * (Ny - 1) - 2];
	double val[3 * (Ny - 1) - 2];
	double rhs[Ny - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bhx;
	val[2] = chx;
	val[3 * N - 3] = bhx;
	val[3 * N - 5] = ahx;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = ahx;
		val[i + 2] = bhx;
		val[i + 4] = chx;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int k = 0; k < Nz - 1; k++)
	{
		for (int i = 0; i < Nx - 1; i++)
		{
			for (int j = 0; j < Ny - 1; j++)
			{
				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (j == 0)
					rhs[j] = bhx*halfgrid_now[k*Nx*Ny + i*Ny + j].bx + chx*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].bx
					+ dt*((halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / dy);
				else
					rhs[j] = ahx*halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx + bhx*halfgrid_now[k*Nx*Ny + i*Ny + j].bx + chx*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].bx
					+ dt*((halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / dz - (halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / dy);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int j1 = 0; j1 < Ny - 1; j1++)
			{
				//�Խ����������
				/*if (k == 0)
				{
				rhs[j1] = -1 * (X*bate / pi)*hm*sin((pi / X)*i*dx)*sin(omega*step*dt);
				}*/
				if (i == 0 || i == Nx - 2)
				{
					rhs[j1] = 0.0;
				}
				//������
				halfgrid_now[k*Nx*Ny + i*Ny + j1].bx = rhs[j1];
			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}

}

void norm_gsscalc_by(Grid* halfgrid_now, int step)
{
	double ahy = (-1 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dz)*(1 / dz));
	double bhy = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / epsl0)*(1 / dz)*(1 / dz));
	double chy = ahy;

	int nRet;//GSS�����ķ���ֵ
	int N = Nz - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nz];
	int ind[3 * (Nz - 1) - 2];
	double val[3 * (Nz - 1) - 2];
	double rhs[Nz - 1];

	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (int i = 0; i < 32; i++)	setting[i] = 0.0;//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��
	val[0] = bhy;
	val[2] = chy;
	val[3 * N - 3] = bhy;
	val[3 * N - 5] = ahy;
	for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
	{
		val[i] = ahy;
		val[i + 2] = bhy;
		val[i + 4] = chy;
		j++;
	}
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int i = 0; i < Nx - 1; i++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int k = 0; k < Nz - 1; k++)
			{
				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (k == 0)
					rhs[k] = bhy*halfgrid_now[k*Nx*Ny + i*Ny + j].by + chy*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].by
					+ dt*((halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / dx - (halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / dz);
				else
					rhs[k] = ahy*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by + bhy*halfgrid_now[k*Nx*Ny + i*Ny + j].by + chy*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].by
					+ dt*((halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / dx - (halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / dz);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int k1 = 0; k1 < Nz - 1; k1++)
			{
				//�Խ����������
				/*if (k1 == 0)
				{
				rhs[k1] = 0.0;
				}*/
				if (j == 0 || j == Ny - 2)
				{
					rhs[k1] = 0.0;
				}

				//������
				halfgrid_now[k1*Nx*Ny + i*Ny + j].by = rhs[k1];

			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}
}


void cpml_gsscalc_ez(Grid* halfgrid_now, int step)
{
	
	//CPML��psi���鲿��
	double psi_ezx[Nx*Ny * 10];
	for (auto i = begin(psi_ezx); i != end(psi_ezx);) { *i++ = 0.0; }//�����ʼ��

	double psi_ezy[Nx*Ny * 10];
	for (auto i = begin(psi_ezy); i != end(psi_ezy);) { *i++ = 0.0; }//�����ʼ��

	int nRet;//GSS�����ķ���ֵ
	constexpr int N = Nx - 1;

	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nx];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }

	//�����ָ��	
	void *hSolver = NULL;

    //���ò�����ʼ��
	double setting[32];
	for (auto i = begin(setting); i!=end(setting);) { *i++ = 0.0; }

	int type = 0;

	//����ptr����  ptr----->ptr�ĳ�����N+1��ptr[i]��¼��i�е�һ������Ԫ��λ��,���һ��Ԫ��ptr[N]=nnz
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}

	//����ind���� ind----->ÿ������Ԫ���б�
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}

	//����ϵ������
	for (int k = s0; k < Nz - 1; k++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int i = 0; i < Nx - 1; i++)
			{
				//pow��������ֵ��
				double kx = 1+( (kxmax-1) * pow((k-s0)*dz,4) )/pow(d,4); 
				double ky = 1 + ((kymax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);


				double alpha_x = alpha_xmax * pow((k - s0)*dz, 2)/ pow(d, 2);
				double sigma_x = sigma_xmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double alpha_y = alpha_ymax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_y = sigma_ymax * pow((k - s0)*dz, 4) / pow(d, 4);

				double bx = pow(e,-1*(sigma_x/(kx+ alpha_x))*(dt/epsl0));
				double ax = sigma_x*(bx-1)/(kx*(sigma_x+kx*sigma_x));

				double by = pow(e, -1 * (sigma_y / (ky + alpha_y))*(dt / epsl0));
				double ay = sigma_y*(by - 1) / (ky*(sigma_y + ky*sigma_y));

				
				//����ϵ������
				double aez = (-1 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / kx)*(1 / dx)*(1 / dx));
				double bez = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / mur0)*(1 / kx)*(1 / dx)*(1 / dx));
				double cez = aez;

				//val���鴦��
				val[0] = bez;
				val[2] = cez;
				val[3 * N - 3] = bez;
				val[3 * N - 5] = aez;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = aez;
					val[i + 2] = bez;
					val[i + 4] = cez;
					j++;
				}
				psi_ezx[k*Nx*Ny + i*Ny + j] = bx * psi_ezx[k*Nx*Ny + i*Ny + j] + ax * (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].by) / dx;

				psi_ezy[k*Nx*Ny + i*Ny + j] = by * psi_ezy[k*Nx*Ny + i*Ny + j] + ay * (halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[k*Nx*Ny + i*Ny + j-1].bx) / dy;



				//������󷽳̵��Ҷ���rhs����
				if (i == 0 && j != 0)
					rhs[i] = bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by) / (dx*kx) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx) / (dy*ky) + psi_ezx[(k - s0)*Nx*Ny + i*Ny + j] - psi_ezy[(k - s0)*Nx*Ny + i*Ny + j]);
				else if (j == 0 && i != 0)
					rhs[i] = aez*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].ez + bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].by) / (dx*kx) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / (dy*ky) + psi_ezx[(k - s0)*Nx*Ny + i*Ny + j] - psi_ezy[(k - s0)*Nx*Ny + i*Ny + j]);
				else if (i == 0 && j == 0)
					rhs[i] = bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by) / (dx*kx) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx) / (dy*ky) + psi_ezx[(k - s0)*Nx*Ny + i*Ny + j] - psi_ezy[(k - s0)*Nx*Ny + i*Ny + j]);
				else if (i != 0 && j != 0)
					rhs[i] = aez*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].ez + bez*halfgrid_now[k*Nx*Ny + i*Ny + j].ez + cez*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].by) / (dx*kx) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx) / (dy*ky) + psi_ezx[(k - s0)*Nx*Ny + i*Ny + j] - psi_ezy[(k - s0)*Nx*Ny + i*Ny + j]);
			}


			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int i1 = 0; i1 < Nx - 1; i1++)
			{
				////�Խ����������								
				//if (j == 0 || j == ny - 2 || i1 == 0 || i1 == nx - 2)//����ez�ı߽����⣬���ĸ����λ��Ӧ��Ϊ0
				//{
				//	rhs[i1] = 0.0;
				//}
				//������	
				
				halfgrid_now[k*Nx*Ny + i1*Ny + j].ez = rhs[i1];
			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);

		}
	}
}

void cpml_gsscalc_ex(Grid* halfgrid_now, int step)
{
	double psi_exy[Nx*Ny * 10];
	for (auto i = begin(psi_exy); i != end(psi_exy);) { *i++ = 0.0; }//�����ʼ��

	double psi_exz[Nx*Ny * 10];
	for (auto i = begin(psi_exz); i != end(psi_exz);) { *i++ = 0.0; }//�����ʼ��

	int nRet;//GSS�����ķ���ֵ
	constexpr int N = Ny - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Ny];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }

	void *hSolver = NULL;//�����ָ��
	double setting[32];//���ò�����ʼ��
	for (auto i = begin(setting); i != end(setting);) { *i++ = 0.0; }

	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int k = s0; k < Nz - 1; k++)
	{
		for (int i = 0; i < Nx - 1; i++)
		{
			for (int j = 0; j < Ny - 1; j++)
			{
				double ky = 1 + ((kymax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);
				double kz = 1 + ((kzmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);


				double alpha_y = alpha_ymax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_y = sigma_ymax * pow((k - s0)*dz, 4) / pow(d, 4);

				double alpha_z = alpha_zmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_z = sigma_zmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double by = pow(e, -1 * (sigma_y / (ky + alpha_y))*(dt / epsl0));
				double ay = sigma_y*(by - 1) / (ky*(sigma_y + ky*sigma_y));

				double bz = pow(e, -1 * (sigma_z / (kz + alpha_z))*(dt / epsl0));
				double az = sigma_z*(by - 1) / (kz*(sigma_z + kz*sigma_z));


				double aex = (-1 * (dt / 2)*(dt / 2)*(1 / (mur0*ky))*(1 / dy)*(1 / dy));
				double bex = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / (mur0*ky))*(1 / dy)*(1 / dy));
				double cex = aex;

				//val���鴦��
				val[0] = bex;
				val[2] = cex;
				val[3 * N - 3] = bex;
				val[3 * N - 5] = aex;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = aex;
					val[i + 2] = bex;
					val[i + 4] = cex;
					j++;
				}
				psi_exy[k*Nx*Ny + i*Ny + j] = by * psi_exy[k*Nx*Ny + i*Ny + j] + ay * (halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + i*Ny + j-1].bz) / dy;

				psi_exz[k*Nx*Ny + i*Ny + j] = bz * psi_exz[k*Nx*Ny + i*Ny + j] + az * (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[(k-1)*Nx*Ny + i*Ny + j].by) / dz;

				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (j == 0)
					rhs[j] = bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / (dy*ky) - (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by) / (dz*kz) + psi_exy[(k-s0)*Nx*Ny + i*Ny + j] - psi_exz[(k - s0)*Nx*Ny + i*Ny + j]);
				else if (j != 0)
					rhs[j] = aex*halfgrid_now[k*Nx*Ny + i*Ny + j - 1].ex + bex*halfgrid_now[k*Nx*Ny + i*Ny + j].ex + cex*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bz) / (dy*ky) - (halfgrid_now[k*Nx*Ny + i*Ny + j].by - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by) / (dz*kz) + psi_exy[(k - s0)*Nx*Ny + i*Ny + j] - psi_exz[(k - s0)*Nx*Ny + i*Ny + j]);
			}//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int j1 = 0; j1 < Ny - 1; j1++)
			{
				//�Խ����������				
				//if (j1 == 0 || j1 == Ny - 2 || k == Nz - 2 || k == 0)
				//{
					//rhs[j1] = 0.0;
				//}
				halfgrid_now[k*Nx*Ny + i*Ny + j1].ex = rhs[j1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}

	}
}

void cpml_gsscalc_ey(Grid* halfgrid_now, int step)
{
	double psi_eyz[Nx*Ny * 10];
	for (auto i = begin(psi_eyz); i != end(psi_eyz);) { *i++ = 0.0; }//�����ʼ��

	double psi_eyx[Nx*Ny * 10];
	for (auto i = begin(psi_eyx); i != end(psi_eyx);) { *i++ = 0.0; }//�����ʼ��


	int nRet;//GSS�����ķ���ֵ
	constexpr int N = 10;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nz];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }

	void *hSolver = NULL;//�����ָ��

	double setting[32];//���ò�����ʼ��
	for (auto i = begin(setting); i != end(setting);) { *i++ = 0.0; }

	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}

	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	
	//rhs�����ʼ��
	for (int i = 0; i < N; i++)
		rhs[i] = 0.0;

	for (int i = 0; i < Nx - 1; i++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int k = s0; k < Nz - 1; k++)
			{
				double kz = 1 + ((kzmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);
				double kx = 1 + ((kxmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);


				double alpha_z = alpha_zmax * pow((kz - s0)*dz, 2) / pow(d, 2);
				double sigma_z = sigma_zmax * pow((kz - s0)*dz, 4) / pow(d, 4);

				double alpha_x = alpha_xmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_x = sigma_xmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double bz = pow(e, -1 * (sigma_z / (kz + alpha_z))*(dt / epsl0));
				double az = sigma_z*(bz - 1) / (kz*(sigma_z + kz*sigma_z));

				double bx = pow(e, -1 * (sigma_x / (kx + alpha_x))*(dt / epsl0));
				double ax = sigma_x*(bx - 1) / (kx*(sigma_x + kx*sigma_x));

				double aey = (-1 * (dt / 2)*(dt / 2)*(1 / (mur0*kz))*(1 / dz)*(1 / dz));
				double bey = (epsl0 + 2 * (dt / 2)*(dt / 2)*(1 / (mur0*kz))*(1 / dz)*(1 / dz));
				double cey = aey;

				//val���鴦��
				val[0] = bey;
				val[2] = cey;
				val[3 * N - 3] = bey;
				val[3 * N - 5] = aey;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = aey;
					val[i + 2] = bey;
					val[i + 4] = cey;
					j++;
				}

				if (k == s0)
				{
					if (i == 0)
					{
						rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
							+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / (dx*kx)+psi_eyz[(k-s0)*Nx*Ny + i*Ny + j]-psi_eyx[(k - s0)*Nx*Ny + i*Ny + j]);

						rhs[k] -= aey*halfgrid_now[(k - 2)*Nx*Ny + i*Ny + j].ey;

					}
					else
					{
						rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
							+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz) / (kx*dx) + psi_eyz[(k - s0)*Nx*Ny + i*Ny + j] - psi_eyx[(k - s0)*Nx*Ny + i*Ny + j]);
						//��z���򣬴��������ϣ�Ҫ�Է����ұ�����д�����ȥA������ܹ������ԽǾ���
						rhs[k] -= aey*halfgrid_now[(k - 2)*Nx*Ny + i*Ny + j].ey;

					}
								
				}
					
				else if (k != s0)
				{
					if (i == 0)
					{
						rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
							+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz) / (dx*kx) + psi_eyz[(k - s0)*Nx*Ny + i*Ny + j] - psi_eyx[(k - s0)*Nx*Ny + i*Ny + j]);

					}
					else
					{
						rhs[k] = aey*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].ey + bey*halfgrid_now[k*Nx*Ny + i*Ny + j].ey + cey*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey
							+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j].bx - halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].bx) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j].bz - halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz) / (dx*kx) + psi_eyz[(k - s0)*Nx*Ny + i*Ny + j] - psi_eyx[(k - s0)*Nx*Ny + i*Ny + j]);
					
					}
				}
					
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);
			//����������뱣��
			for (int k1 = 0; k1 < Nz - 1; k1++)
			{
				//if (k1 == 0) //�Խ����������
				//{//���ı�Դ��ֵ					
				//	if (step*dt < 2 * T)
				//	{
				//		double rising_edge = (step*dt) / (2 * T);
				//		double temp_ey = hm * sin((pi / X)*i*dx);

				//		rhs[k1] = rising_edge * temp_ey * sin(omega*step*dt);//ey
				//	}
				//	else
				//	{
				//		double temp_ey = hm * sin((pi / X)*i*dx);

				//		rhs[k1] = temp_ey*sin(omega*step*dt);//ey
				//	}
					//double temp_ey = sin((pi / X)*i*dx);
					// 
					//rhs[k1] = temp_ey*sin(omega*step*dt);//ey
				//}

				/*if (i == 0 || i == Nx - 2 || k1 == Nz - 2)
				{
					rhs[k1] = 0.0;
				}*/
				//������
				halfgrid_now[k1*Nx*Ny + i*Ny + j].ey = rhs[k1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}
}

void cpml_gsscalc_bz(Grid* halfgrid_now, int step)
{
	double psi_hzy[Nx*Ny * 10];
	for (auto i = begin(psi_hzy); i != end(psi_hzy);) { *i++ = 0.0; }//�����ʼ��

	double psi_hzx[Nx*Ny * 10];
	for (auto i = begin(psi_hzx); i != end(psi_hzx);) { *i++ = 0.0; }//�����ʼ��


	int nRet;//GSS�����ķ���ֵ
	constexpr int N = Nx - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nx];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }


	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (auto i = begin(setting); i != end(setting);) { *i++ = 0.0; }

	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	//val���鴦��

	for (int k = s0; k < Nz - 1; k++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int i = 0; i < Nx - 1; i++)
			{

				double kx = 1 + ((kxmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);
				double ky = 1 + ((kymax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);


				double alpha_x = alpha_xmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_x = sigma_xmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double alpha_y = alpha_ymax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_y = sigma_ymax * pow((k - s0)*dz, 4) / pow(d, 4);

				double bx = pow(e, -1 * (sigma_x / (kx + alpha_x))*(dt / epsl0));
				double ax = sigma_x*(bx - 1) / (kx*(sigma_x + kx*sigma_x));

				double by = pow(e, -1 * (sigma_y / (ky + alpha_y))*(dt / epsl0));
				double ay = sigma_y*(by - 1) / (ky*(sigma_y + ky*sigma_y));


			
				double ahz = (-1 * (dt / 2)*(dt / 2)*(1 / (epsl0*kx))*(1 / dx)*(1 / dx));
				double bhz = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / (epsl0*kx))*(1 / dx)*(1 / dx));
				double chz = ahz;
				//���ȴ�����󷽳̵��Ҷ���rhs����

				val[0] = bhz;
				val[2] = chz;
				val[3 * N - 3] = bhz;
				val[3 * N - 5] = ahz;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = ahz;
					val[i + 2] = bhz;
					val[i + 4] = chz;
					j++;
				}
				if (i == 0)
					rhs[i] = bhz*halfgrid_now[k*Nx*Ny + i*Ny + j].bz + chz*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].bz
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / (dy*ky) - (halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / (dx*kx)+psi_hzy[(k-s0)*Nx*Ny + i*Ny + j]-psi_hzx[(k - s0)*Nx*Ny + i*Ny + j]);
				else
					rhs[i] = ahz*halfgrid_now[k*Nx*Ny + (i - 1)*Ny + j].bz + bhz*halfgrid_now[k*Nx*Ny + i*Ny + j].bz + chz*halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].bz
					+ dt*((halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / (dy*ky) - (halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / (dx*kx) + psi_hzy[(k - s0)*Nx*Ny + i*Ny + j] - psi_hzx[(k - s0)*Nx*Ny + i*Ny + j]);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;	//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);
			for (int i1 = 0; i1 < Nx - 1; i1++)
			{
				//�Խ����������				
				//if (k == 0)//���ı�����Դ��ֵ
				//{
				//	double rising_edge = (step*dt) / (2 * T);

				//	if (step*dt < 2 * T)
				//	{
				//		double temp_bz = hm * cos((pi / X) * i1*dx);
				//	
				//		rhs[i1]= rising_edge * temp_bz * cos(omega*step*dt);//hz
				//	}
				//	else
				//	{
				//		double temp_bz = hm * cos((pi / X) * i1 * dx);

				//		rhs[i1] =  temp_bz * cos(omega * step * dt);//hz
				//	}
				//	
				//}
				//if (k == Nz - 2)//���ҽ����Դ����ʹ�丳ֵΪ0�������൱���������ձ߽�
				//{
				//	rhs[i1] = 0.0;
				//}
				//������
				halfgrid_now[k*Nx*Ny + i1*Ny + j].bz = rhs[i1];
			}

			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}

}

void cpml_gsscalc_bx(Grid* halfgrid_now, int step)
{
	double psi_hxz[Nx*Ny * 10];
	for (auto i = begin(psi_hxz); i != end(psi_hxz);) { *i++ = 0.0; }//�����ʼ��

	double psi_hxy[Nx*Ny * 10];
	for (auto i = begin(psi_hxy); i != end(psi_hxy);) { *i++ = 0.0; }//�����ʼ��



	int nRet;//GSS�����ķ���ֵ
	constexpr int N = Ny - 1;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Ny];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }


	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (auto i = begin(setting); i != end(setting);) { *i++ = 0.0; }
	//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	for (int k = s0; k < Nz - 1; k++)
	{
		for (int i = 0; i < Nx - 1; i++)
		{
			for (int j = 0; j < Ny - 1; j++)
			{
				double kz = 1 + ((kzmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);
				double ky = 1 + ((kymax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);


				double alpha_z = alpha_zmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_z = sigma_zmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double alpha_y = alpha_ymax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_y = sigma_ymax * pow((k - s0)*dz, 4) / pow(d, 4);

				double bz = pow(e, -1 * (sigma_z / (kz + alpha_z))*(dt / epsl0));
				double az = sigma_z*(bz - 1) / (kz*(sigma_z + kz*sigma_z));

				double by = pow(e, -1 * (sigma_y / (ky + alpha_y))*(dt / epsl0));
				double ay = sigma_y*(by - 1) / (ky*(sigma_y + ky*sigma_y));


				double ahx = (-1 * (dt / 2)*(dt / 2)*(1 / (epsl0*ky))*(1 / dy)*(1 / dy));
				double bhx = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / (epsl0*ky))*(1 / dy)*(1 / dy));
				double chx = ahx;

				//val���鴦��
				val[0] = bhx;
				val[2] = chx;
				val[3 * N - 3] = bhx;
				val[3 * N - 5] = ahx;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = ahx;
					val[i + 2] = bhx;
					val[i + 4] = chx;
					j++;
				}

				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (j == 0)
					rhs[j] = bhx*halfgrid_now[k*Nx*Ny + i*Ny + j].bx + chx*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].bx
					+ dt*((halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / (dy*ky) + psi_hxz[(k - s0)*Nx*Ny + i*Ny + j] - psi_hxy[(k - s0)*Nx*Ny + i*Ny + j]);
				else
					rhs[j] = ahx*halfgrid_now[k*Nx*Ny + i*Ny + j - 1].bx + bhx*halfgrid_now[k*Nx*Ny + i*Ny + j].bx + chx*halfgrid_now[k*Nx*Ny + i*Ny + j + 1].bx
					+ dt*((halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ey - halfgrid_now[k*Nx*Ny + i*Ny + j].ey) / (dz*kz) - (halfgrid_now[k*Nx*Ny + i*Ny + j + 1].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / (dy*ky) + psi_hxz[(k - s0)*Nx*Ny + i*Ny + j] - psi_hxy[(k - s0)*Nx*Ny + i*Ny + j]);
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int j1 = 0; j1 < Ny - 1; j1++)
			{
				//�Խ����������
				/*if (k == 0)
				{
				rhs[j1] = -1 * (X*bate / pi)*hm*sin((pi / X)*i*dx)*sin(omega*step*dt);
				}*/
				/*if (i == 0 || i == Nx - 2)
				{
					rhs[j1] = 0.0;
				}*/
				//������
				halfgrid_now[k*Nx*Ny + i*Ny + j1].bx = rhs[j1];
			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}

}

void cpml_gsscalc_by(Grid* halfgrid_now, int step)
{
	double psi_hyx[Nx*Ny * 10];
	for (auto i = begin(psi_hyx); i != end(psi_hyx);) { *i++ = 0.0; }//�����ʼ��

	double psi_hyz[Nx*Ny * 10];
	for (auto i = begin(psi_hyz); i != end(psi_hyz);) { *i++ = 0.0; }//�����ʼ��

	int nRet;//GSS�����ķ���ֵ
	constexpr int N = 10;
	int nnz = 3 * N - 2;
	int nRow = N;
	int nCol = N;

	int ptr[Nz];
	int ind[3 * N - 2];
	double val[3 * N - 2];
	double rhs[N];
	//rhs�����ʼ��,�����Ҷ˳�����
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }


	void *hSolver = NULL;//�����ָ��
	double setting[32];
	for (auto i = begin(rhs); i != end(rhs);) { *i++ = 0.0; }

	//���ò�����ʼ��
	int type = 0;

	//����ptr����
	ptr[0] = 0;
	ptr[1] = 2;
	ptr[N] = 3 * N - 2;
	for (int i = 2; i < N; i++)
	{
		ptr[i] = ptr[i - 1] + 3;
	}
	//����ind����
	ind[0] = 0;
	ind[1] = 1;
	ind[3 * N - 3] = N - 1;
	ind[3 * N - 4] = N - 2;
	for (int i = 2, j = 0; i + 2 < 3 * N - 4; i = i + 3)
	{
		ind[i] = j;
		ind[i + 1] = j + 1;
		ind[i + 2] = j + 2;
		j++;
	}
	
	for (int i = 0; i < Nx - 1; i++)
	{
		for (int j = 0; j < Ny - 1; j++)
		{
			for (int k = s0; k < Nz - 1; k++)
			{
				double kx = 1 + ((kxmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);
				double kz = 1 + ((kzmax - 1) * pow((k - s0)*dz, 4)) / pow(d, 4);

				double alpha_x = alpha_xmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_x = sigma_xmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double alpha_z = alpha_zmax * pow((k - s0)*dz, 2) / pow(d, 2);
				double sigma_z = sigma_zmax * pow((k - s0)*dz, 4) / pow(d, 4);

				double bx = pow(e, -1 * (sigma_x / (kx + alpha_x))*(dt / epsl0));
				double ax = sigma_x*(bx - 1) / (kx*(sigma_x + kx*sigma_x));

				double bz = pow(e, -1 * (sigma_z / (kz + alpha_z))*(dt / epsl0));
				double az = sigma_z*(bz - 1) / (kz*(sigma_z + kz*sigma_z));

				double ahy = (-1 * (dt / 2)*(dt / 2)*(1 / (epsl0*kx))*(1 / dz)*(1 / dz));
				double bhy = (mur0 + 2 * (dt / 2)*(dt / 2)*(1 / (epsl0*kx))*(1 / dz)*(1 / dz));
				double chy = ahy;

				//val���鴦��
				val[0] = bhy;
				val[2] = chy;
				val[3 * N - 3] = bhy;
				val[3 * N - 5] = ahy;
				for (int i = 1, j = 2; i + 3 < 3 * N - 2; i = i + 3)
				{
					val[i] = ahy;
					val[i + 2] = bhy;
					val[i + 4] = chy;
					j++;
				}

				//���ȴ�����󷽳̵��Ҷ���rhs����
				if (k == s0)
				{
					rhs[k] = ahy*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by + bhy*halfgrid_now[k*Nx*Ny + i*Ny + j].by + chy*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].by
					+ dt*((halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / (dx*kx) - (halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / (dz*kz) + psi_hyx[(k - s0)*Nx*Ny + i*Ny + j] - psi_hyz[(k - s0)*Nx*Ny + i*Ny + j]);

					rhs[k] -= ahy*halfgrid_now[(k - 2)*Nx*Ny + i*Ny + j].by;
				}
				else
				{
					rhs[k] = ahy*halfgrid_now[(k - 1)*Nx*Ny + i*Ny + j].by + bhy*halfgrid_now[k*Nx*Ny + i*Ny + j].by + chy*halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].by
					+ dt*((halfgrid_now[k*Nx*Ny + (i + 1)*Ny + j].ez - halfgrid_now[k*Nx*Ny + i*Ny + j].ez) / (dx*kx) - (halfgrid_now[(k + 1)*Nx*Ny + i*Ny + j].ex - halfgrid_now[k*Nx*Ny + i*Ny + j].ex) / (dz*kz) + psi_hyx[(k - s0)*Nx*Ny + i*Ny + j] - psi_hyz[(k - s0)*Nx*Ny + i*Ny + j]);
				}
			}
			//GSS���
			nRet = GSS_init_ld(nRow, nCol, ptr, ind, val, type, setting);
			if (nRet != GRUS_OK) {
				printf("\tERROR at init GSS solver. ERROR CODE:%d\r\n", nRet);
				return;
			}

			hSolver = GSS_symbol_ld(nRow, nCol, ptr, ind, val);
			if (hSolver == NULL) {
				printf("\tERROR at SYMBOLIC ANALYSIS.\r\n");
				exit(0);
			}

			nRet = GSS_numeric_ld(nRow, nCol, ptr, ind, val, hSolver);
			if (nRet != GRUS_OK) {
				printf("\r\n\tERROR at NUMERIC FACTORIZATION. ERROR CODE:%d\r\n", nRet);
				hSolver = NULL;		//��������ΪNULL,GSS���Զ��ͷ��ڴ�
				exit(0);
			}

			GSS_solve_ld(hSolver, nRow, nCol, ptr, ind, val, rhs);

			for (int k1 = 0; k1 < Nz - 1; k1++)
			{
				//�Խ����������
				/*if (k1 == 0)
				{
				rhs[k1] = 0.0;
				}*/
				/*if (j == 0 || j == Ny - 2)
				{
					rhs[k1] = 0.0;
				}*/

				//������
				halfgrid_now[k1*Nx*Ny + i*Ny + j].by = rhs[k1];

			}
			if (hSolver != NULL)
				GSS_clear_ld(hSolver);
		}
	}

}

