#pragma once

#include<set>	    
#include<map>      
#include<utility>  //pair�����ڵ�ͷ�ļ�
#include<string>
#include<vector>
#include<iostream>
#include<iomanip>
#include<fstream>  
#include<sstream>
#include<algorithm>
#include<iterator>

using namespace std;
typedef unsigned int u_int;

class Apriori {
private:
	string in;
	string out;
	double minSup; //֧�ֶ���ֵ
	double minConf; //���Ŷ���ֵ
	u_int dataBaseCnt; //������
	map< set<string>, u_int >  supLabel; //֧�ֶȼ���
	vector< set<string> > dataSet; //ԭʼ���ݼ�
	map < pair<set<string>, set<string> >, double > rule; //��������
	void readData(); //��ȡ����
	map< set<string>, u_int>  getSingle(); //�õ�����Ƶ���
	set<set<string> > getCandiSet(map<set<string>, u_int> item); //��Ƶ��k-1��õ���ѡk�
	map< set<string>, u_int> getFreqSet(set<set<string> > item); //�ɺ�ѡ��õ�Ƶ���
	void genRules(map< set<string>, u_int> items ); //��Ƶ������ɹ�������
	map<pair<set<string>, set<string> >, double> genRule_begin(set<string>); //����1������������
	void ap_genRules(set<string>, map<pair<set<string>, set<string> >, double >, int k); //��k����������������k+1���������
	void showFreqItem(u_int k, map<set<string>, u_int > item); //���Ƶ���
	void showRules(); //�����������
	bool subSet(set<string> a, set<string> b); //�ж�a�Ƿ�Ϊb���Ӽ�
public:
	Apriori(string s1, string s2, double mSup, double mConf) {
		in = s1;
		out = s2;
		minSup = mSup;
		minConf = mConf;
		dataBaseCnt = 0;
	}
	~Apriori() {};
	void process();//�����㷨���

};

//���ļ���ȡ����
void Apriori::readData() {
	ifstream inf(in); //���ļ������������
	if (!inf.is_open()) {
		cout << "Error opening file!\n";
		exit(1);
	}
	string str;
	dataBaseCnt = 0;
	while (getline(inf, str)) {
		set<string> line;
		stringstream input(str);  //�ַ���������
		string res;
		while (getline(input,res,',')) { //�����ŷָ�������
			line.insert(res);
		}
		dataSet.push_back(line);
		dataBaseCnt++;
	}

	inf.close();
}

//�õ�����Ƶ���
map<set<string>, u_int> Apriori::getSingle() {
	set<string>  C1;
	for (vector<set<string> >::iterator i = dataSet.begin(); i != dataSet.end(); i++) {
		for (set<string>::iterator it = (*i).begin(); it != (*i).end(); it++) {
			if (C1.count(*it) == 0) {
				C1.insert(*it);
			}
			set<string> t{ *it };
			supLabel[t]++;
		}
	}
	map<set<string>, u_int> L1;
	for (set<string>::iterator i = C1.begin(); i != C1.end(); i++) {
		set<string> t{ *i };
		if (supLabel[t] >= minSup * dataBaseCnt) {
			L1[t] = supLabel[t];
		}
	}
	return L1;
}

//��k-1-Ƶ����õ�k-��ѡ�
set<set<string> > Apriori::getCandiSet(map<set<string>, u_int> item) {
	set<set<string> > res;
	map<set<string>, u_int>::iterator i = item.begin();
	while (i != item.end()) {
		map<set<string>, u_int>::iterator next = i;
		++next;
		while (next != item.end()) {
			set<string> check_item1 = i->first;
			set<string> check_item2 = next->first;
			set<string>::iterator last_pos1 = check_item1.end();
			set<string>::iterator last_pos2 = check_item2.end();
			string last1 = *(--last_pos1);
			string last2 = *(--last_pos2);
			check_item1.erase(last_pos1);
			check_item2.erase(last_pos2);
			set<string> t;
			if (check_item1 == check_item2 && last1 != last2) {
				t = check_item1;
				t.insert(last1);
				t.insert(last2);
			}
			if (t.size())	res.insert(t);
			++next;
		}
		++i;
	}
	return res;
}

//�ɺ�ѡ��õ�Ƶ���
map<set<string>, u_int> Apriori::getFreqSet(set<set<string> > candiSet) {
	vector<set<string> >::iterator i;
	set<set<string> >::iterator check;
	map<set<string>, u_int > res;
	for (check = candiSet.begin(); check != candiSet.end(); ++check) {
		for (i = dataSet.begin(); i != dataSet.end(); ++i) {
			if (subSet(*check, *i)) {
				++supLabel[*check];
			}
		}
		if (supLabel[*check] >= minSup * dataBaseCnt) {
			res[*check] = supLabel[*check];
		}
	}
	return res;
}

//�ж�a�Ƿ�b���Ӽ�
bool Apriori::subSet(set<string> a, set<string> b) {
	set<string> t;
	set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(t, t.begin()));
	if (!t.size()) {
		return true;
	}
	return false;
}

//����1�����Ĺ�������
map<pair<set<string>, set<string> >, double> Apriori::genRule_begin(set<string> freqItem) {
	map<pair<set<string>, set<string>>, double> temp;
	set<string>::iterator consequent = freqItem.begin();
	while (consequent != freqItem.end()) {
		set<string> a{ *consequent };
		set<string> t;
		set_difference(freqItem.begin(), freqItem.end(), a.begin(), a.end(), inserter(t, t.begin()));
		double conf = 1.0 * supLabel[freqItem] / supLabel[t];
		if (conf >= minConf) {
			pair<set<string>, set<string> > P = make_pair(t, a);
			rule[P] = conf;
			temp[P] = conf;
		}
		++consequent;
	}
	return temp;
}

//��k-1����������������k������������ (�ݹ�����)
void Apriori::ap_genRules(set<string> item, map<pair<set<string>, set<string> >, double> H, int k) {
	if (k == item.size()) return;
	map<pair<set<string>, set<string>>, double>::iterator i = H.begin();
	map<pair<set<string>, set<string>>, double> temp;
	while (i != H.end()) {
		map<pair<set<string>, set<string>>, double>::iterator next = i;
		++next;
		while (next != H.end()) {
			pair<set<string>, set<string>> P1 = i->first;
			pair<set<string>, set<string>> P2 = next->first;
			set<string> antecedent = P1.first;
			set<string> consequent1 = P1.second;
			set<string> consequent2 = P2.second;
			set<string>::iterator last1 = consequent1.end();
			set<string>::iterator last2 = consequent2.end();
			string s1 = *(--last1);
			string s2 = *(--last2);
			consequent1.erase(last1);
			consequent2.erase(last2);
			set<string> at;
			set<string> ct;
			if (consequent1 == consequent2&&s1!=s2) {
				ct = consequent1;
				ct.insert(s1);
				ct.insert(s2);
				set_difference(antecedent.begin(), antecedent.end(), ct.begin(), ct.end(), inserter(at, at.begin()));
				double conf = 1.0 * supLabel[item] / supLabel[at];
				if (conf >= minConf) {
					rule[make_pair(at, ct)] = conf;
					temp[make_pair(at, ct)] = conf;
				}
			}
			++next;
		}
		++i;
	}
	ap_genRules(item, temp, k + 1);
}

//���Ƶ�����֧�ֶ�
void Apriori::showFreqItem(u_int k, map<set<string>, u_int> item) {
	map<set<string>, u_int>::iterator i = item.begin();
	ofstream inf(out, ios::app);
	inf << endl << "Ƶ��" << k << "�Ϊ:" << endl;
	inf <<left<< setw(30)<<"�� ��" <<right<<setw(30)<< "֧�ֶ�" << endl;
	while (i != item.end()) {
		set<string> s = i->first;
		set<string>::iterator j = s.begin();
		inf <<"{";
		while (j != s.end()) {
			inf << *j << ",";
			++j;
		}
		inf << "}" ;
		inf <<setw(30)<<i->second * 1.0 / dataBaseCnt << endl;
		++i;
	}
	inf.close();
}

//��������������Ŷ�
void Apriori::showRules() {
	ofstream inf(out, ios::app);
	inf << endl << "��������:" << endl;
	inf <<left<<setw(30)<<"����" <<right<<setw(30)<< "���Ŷ�" << endl;
	map<pair<set<string>, set<string>>, double>::iterator i = rule.begin();
	while (i != rule.end()) {
		pair<set<string>, set<string> > P = i->first;
		set<string> antecedent = P.first;
		set<string> consequent = P.second;
		set<string>::iterator s1 = antecedent.begin();
		set<string>::iterator s2 = consequent.begin();
		inf << "{";
		while (s1 != antecedent.end()) {
			inf << *s1 << ",";
			++s1;
		}
		inf << "}";
		inf << "==>>";
		inf << "{";
		while (s2 != consequent.end()) {
			inf << *s2 << ",";
			++s2;
		}
		inf << "}";
		inf << setw(30)<<i->second;
		inf << endl;
		++i;
	}
	inf.close();
}

//�ж�a�Ƿ�Ϊb���Ӽ�
void Apriori::genRules(map< set<string>, u_int> items ) {
	map<pair<set<string>, set<string>>, double> t;
	map<set<string>, u_int>::iterator i = items.begin();
	while (i != items.end()) {
		set<string> item = i->first;
		t = genRule_begin(item);
		ap_genRules(item, t, 2);
		++i;
	}
}

//�㷨������
void Apriori::process() {
	readData();
	cout << "��" << in << "��ȡ�ɹ�..." << endl;
	map<set<string>, u_int> k_item;
	k_item = getSingle();
	cout << "����Ƶ����������..." << endl;
	showFreqItem(1, k_item);
	int i = 1;
	while (true) {
		set<set<string> > ck_item;
		ck_item = getCandiSet(k_item);
		cout << i + 1 << "����ѡ��������..." << endl;
		k_item = getFreqSet(ck_item);
		cout << i + 1 << "��Ƶ����������..." << endl;
		if (k_item.size()) {
			genRules(k_item);
			cout << "��" << i + 1<<"��Ƶ������ɹ���������� ..."<< endl;
		}
		else {
			cout << i + 1 << "��Ƶ���Ϊ��,���µ�Ƶ�������." << endl;
			break;
		}
		showFreqItem(++i, k_item);
	}
	showRules();
	cout << "�������,���������" << out << endl;
}