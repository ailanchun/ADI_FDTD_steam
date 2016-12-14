#include"cstdio.h"
#include"cmath.h"
#include"iostream.h"
#include"definer.h"
#include"functions.h"

int trade(double*b, int n, int m, double*d)
{
	if (m != 3 * n - 2)//�����ж��Ƿ��������ԽǾ���Ľṹ����
	{
		cout << "�����������ԽǾ����������" << endl;
		return -2;
	}
	for (int k = 0; k <= n - 2; k++)
	{
		int j = k * 3;
		double s = b[j];
		if (fabs(s) + 1.0 == 1.0)//fabs()�������󸡵����ľ���ֵ����������Ϊdouble
		{
			cout << "��ĸΪ0���������" << endl;
			return -1;
		}
		b[j + 1] = b[j + 1] / s;//ϵ�������һ��
		d[k] = d[k] / s;//����������һ��
		b[j + 3] = b[j + 3] - b[j + 2] * b[j + 1];//ϵ��������Ԫ
		d[k + 1] = d[k + 1] - b[j + 2] * d[k];
	}
	s = b[3 * n - 3];
	if (fabs(s) + 1.0 == 1.0)
	{
		cout << "��ĸΪ0����������" << endl;
		return -1;
	}
	d[n - 1] = d[n - 1] / s;//�ش������x(n-1)
	for (k = n - 2; k >= 0; k--)
	{
		d[k] = d[k] - b[3 * k + 1] * d[k + 1];
	}
	return 2;

}