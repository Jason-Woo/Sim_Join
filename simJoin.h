#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <map>
#include <math.h> 
#include <set>


using namespace std;

template<typename T1, typename T2, typename T3>
struct triple
{
	T1 id1;
	T2 id2;
	T3 ed;
};

class inverted_list
{
private:
	int cnt;
	vector<int> len_list;
	map<int, vector<map<string, vector<int>>>> list;

public:
	inverted_list(int c)
	{
		cnt = c;
	}
	void insert_ele(int len, int c, string s, int id);
	map<string, vector<int>> get_ele(int len, int c);
	bool exist(int len);
	void print(int c);
};

class simJoin
{
public:
	simJoin(const string& filename)
	{
		readData(filename);
	};

	~simJoin() {};

	bool SimilarityJoin(unsigned ed_threshold, vector< triple<unsigned, unsigned, unsigned> >& results);
	bool getString(int id, string& out) const;
	int getDataNum() const;

private:
	vector<string> data;
	vector<int> len_data;
	int ed_threshold;
	bool readData(const string& filename);
	int edit_distance(string s1, string s2);
	void select_substring(string s, map<string, vector<int>> dict, int str_l, int& cur_pos, int i, set<int>& candidate);
};
#pragma once
