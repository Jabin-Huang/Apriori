#pragma once

#include<set>	    
#include<map>      
#include<utility>  //pair类所在的头文件
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
	double minSup; //支持度阈值
	double minConf; //置信度阈值
	u_int dataBaseCnt; //事务数
	map< set<string>, u_int >  supLabel; //支持度计数
	vector< set<string> > dataSet; //原始数据集
	map < pair<set<string>, set<string> >, double > rule; //关联规则
	void readData(); //读取数据
	map< set<string>, u_int>  getSingle(); //得到单项频繁项集
	set<set<string> > getCandiSet(map<set<string>, u_int> item); //由频繁k-1项集得到侯选k项集
	map< set<string>, u_int> getFreqSet(set<set<string> > item); //由侯选项集得到频繁项集
	void genRules(map< set<string>, u_int> items ); //由频繁项集生成关联规则
	map<pair<set<string>, set<string> >, double> genRule_begin(set<string>); //生成1项后件关联规则
	void ap_genRules(set<string>, map<pair<set<string>, set<string> >, double >, int k); //由k项后件关联规则生成k+1项关联规则
	void showFreqItem(u_int k, map<set<string>, u_int > item); //输出频繁项集
	void showRules(); //输出关联规则
	bool subSet(set<string> a, set<string> b); //判断a是否为b的子集
public:
	Apriori(string s1, string s2, double mSup, double mConf) {
		in = s1;
		out = s2;
		minSup = mSup;
		minConf = mConf;
		dataBaseCnt = 0;
	}
	~Apriori() {};
	void process();//整体算法框架

};

//从文件读取数据
void Apriori::readData() {
	ifstream inf(in); //从文件输入的流对象
	if (!inf.is_open()) {
		cout << "Error opening file!\n";
		exit(1);
	}
	string str;
	dataBaseCnt = 0;
	while (getline(inf, str)) {
		set<string> line;
		stringstream input(str);  //字符串流对象
		string res;
		while (getline(input,res,',')) { //按逗号分割行输入
			line.insert(res);
		}
		dataSet.push_back(line);
		dataBaseCnt++;
	}

	inf.close();
}

//得到单项频繁项集
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

//由k-1-频繁项集得到k-侯选项集
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

//由侯选项集得到频繁项集
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

//判断a是否b的子集
bool Apriori::subSet(set<string> a, set<string> b) {
	set<string> t;
	set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(t, t.begin()));
	if (!t.size()) {
		return true;
	}
	return false;
}

//生成1项后件的关联规则
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

//由k-1项后件关联规则生成k项后件关联规则 (递归生成)
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

//输出频繁项集及支持度
void Apriori::showFreqItem(u_int k, map<set<string>, u_int> item) {
	map<set<string>, u_int>::iterator i = item.begin();
	ofstream inf(out, ios::app);
	inf << endl << "频繁" << k << "项集为:" << endl;
	inf <<left<< setw(30)<<"项 集" <<right<<setw(30)<< "支持度" << endl;
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

//输出关联规则及置信度
void Apriori::showRules() {
	ofstream inf(out, ios::app);
	inf << endl << "关联规则:" << endl;
	inf <<left<<setw(30)<<"规则" <<right<<setw(30)<< "可信度" << endl;
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

//判断a是否为b的子集
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

//算法整体框架
void Apriori::process() {
	readData();
	cout << "从" << in << "读取成功..." << endl;
	map<set<string>, u_int> k_item;
	k_item = getSingle();
	cout << "单项频繁项集处理完成..." << endl;
	showFreqItem(1, k_item);
	int i = 1;
	while (true) {
		set<set<string> > ck_item;
		ck_item = getCandiSet(k_item);
		cout << i + 1 << "级侯选项集生成完毕..." << endl;
		k_item = getFreqSet(ck_item);
		cout << i + 1 << "级频繁项集生成完毕..." << endl;
		if (k_item.size()) {
			genRules(k_item);
			cout << "由" << i + 1<<"级频繁项集生成关联规则完毕 ..."<< endl;
		}
		else {
			cout << i + 1 << "级频繁项集为空,无新的频繁项集生成." << endl;
			break;
		}
		showFreqItem(++i, k_item);
	}
	showRules();
	cout << "处理完毕,结果保存在" << out << endl;
}