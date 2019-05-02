#include<iostream>
#include"Apriori.h"

using namespace std;

int main() {
	/*cout << "输入数据源文件名:" << endl;
	string filename1,filename2;
	double minConf;
	double minSup;
	cin >> filename1;
	cout << "输入支持度阈值:" << endl;
	cin >> minSup;
	cout << "输入置信度阈值:" << endl;
	cin >> minConf;
	cout << "输入结果导出文件名" << endl;
	cin >> filename2;*/
	Apriori A = Apriori("a.txt", "b.txt", 0.2, 0.2);
	A.process();

	return 0;
}