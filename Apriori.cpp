#include<iostream>
#include"Apriori.h"

using namespace std;

int main() {
	cout << "��������Դ�ļ���:" << endl;
	string filename1,filename2;
	double minConf;
	double minSup;
	cin >> filename1;
	cout << "����֧�ֶ���ֵ:" << endl;
	cin >> minSup;
	cout << "�������Ŷ���ֵ:" << endl;
	cin >> minConf;
	cout << "�����������ļ���" << endl;
	cin >> filename2;
	Apriori A = Apriori(filename1, filename2, minSup, minConf);
	A.process();

	return 0;
}