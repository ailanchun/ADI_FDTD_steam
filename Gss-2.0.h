/*
	����GSS2.0���ϡ�����
	ϡ��������ѹ���и�ʽ�洢���±��0��ʼ

	ѹ���и�ʽ���ã�int* ptr,int* ind,double *val����¼ϡ�����ķ���Ԫ��
	����N����nnz������Ԫ�ľ�����˵:
	ind, val�ĳ�����nnz������˳���¼ÿ������Ԫ���б����ֵ��
	ptr�ĳ�����N+1��ptr[i]��¼��i�е�һ������Ԫ��λ��,���һ��Ԫ��ptr[N]=nnz��
		������i�еĳ��Ⱦ���   ptr[i+1]-ptr[i]��

	�����������3�׵ľ���
		1.0  0.0  5.0
		0.0  3.0  6.0
		2.0  4.0  7.0
   �� 	ptr[4]={0,2,4,7};
		ind[7]={0,2,1,2,0,1,2};
		val[7]={1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

   Copyright (c) 2005-present by YingShiChen.    All Rights Reserved. 
	Any problem,contact gsp@grusoft.com
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GSS_6_IMPORT __declspec(dllimport)		//�������к�����DLL������

#ifdef __cplusplus
extern "C" {
#endif
	#define GRUS_MF_STATUS			0	
	#define GRUS_OK					0

/*
	��ʼ��

	int nRow,nCol	����������
	int *ptr, int *ind, double *val	����ѹ���и�ʽ������(ptr,ind,val)Ϊ��ָ�룬���±��Ԫ��ֵ	
	type	�������ͣ��������¡�	
		0��		ȱʡֵΪ0
		11��	�Գ���������(ptr, ind, val)Ϊ����������(���Խ���)�����ݡ�
		12��	�ԳƲ�������(ptr, ind, val)Ϊ����������(���Խ���)�����ݡ�
	setting[32]		���Ʋ�����

	����ֵ��
		��ʼ���ɹ�����GRUS_OK,���򷵻ش������
*/
GSS_6_IMPORT	int GSS_init_ld(int nRow, int nCol, int* ptr, int* ind, double *val, int type, double *setting);

/*
	���ŷֽ�

	int nRow,nCol	����������
	int *ptr, int *ind, double *val	����ѹ���и�ʽ������(ptr,ind,val)Ϊ��ָ�룬���±��Ԫ��ֵ	

	����ֵ��
					�ֽ�ɹ������������ָ��,���򷵻�0x0
*/
GSS_6_IMPORT	void* GSS_symbol_ld(int nRow, int nCol, int* ptr, int* ind, double *val);

/*
	LU��ֵ�ֽ�

	int nRow,nCol	����������
	int *ptr, int *ind, double *val	����ѹ���и�ʽ������(ptr,ind,val)Ϊ��ָ�룬���±��Ԫ��ֵ	
	void *hSolver		ָ���������ָ��

	����ֵ��
					�ֽ�ɹ�����GRUS_OK, ���򷵻ش������
*/
GSS_6_IMPORT	int GSS_numeric_ld( int nRow,int nCol,int* ptr,int* ind,double *val,void *hSolver );

/*
	�ش����

	void *hSolver	ָ���������ָ��
	int nRow,nCol	����������
	int *ptr, int *ind, double *val	����ѹ���и�ʽ������(ptr,ind,val)Ϊ��ָ�룬���±��Ԫ��ֵ	
	double *rhs		��������Ҷ�����صĽ�Ҳ�洢��rhs��

	����ֵ��
					�����ⷵ��GRUS_OK, ���򷵻ش������
*/
GSS_6_IMPORT	int GSS_solve_ld( void *hSolver,int nRow,int nCol,int *ptr,int *ind,double *val,double *rhs );

/*
	�ͷ������ռ�õ��ڴ�

	void *hSolver	ָ���������ָ��

	����ֵ��
					����ͷŷ���GRUS_OK, ���򷵻ش������
*/
GSS_6_IMPORT	int GSS_clear_ld( void* hSolver );

#ifdef __cplusplus
}
#endif


