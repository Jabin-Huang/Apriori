#include<iostream>
#include"Apriori.h"

using namespace std;

int main() {
	/*cout << "��������Դ�ļ���:" << endl;
	string filename1,filename2;
	double minConf;
	double minSup;
	cin >> filename1;
	cout << "����֧�ֶ���ֵ:" << endl;
	cin >> minSup;
	cout << "�������Ŷ���ֵ:" << endl;
	cin >> minConf;
	cout << "�����������ļ���" << endl;
	cin >> filename2;*/
	Apriori A = Apriori("a.txt", "b.txt", 0.2, 0.2);
	A.process();

	return 0;
}