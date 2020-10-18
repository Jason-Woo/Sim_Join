#include "simJoin.h"

void inverted_list::insert_ele(int len, int c, string s, int id)
{
	vector<int>::iterator it = find(len_list.begin(), len_list.end(), len);
	if (it == len_list.end())
	{
		len_list.push_back(len);
		for (int i = 0; i < cnt; i++)
		{
			map<string, vector<int>> ele;
			list[len].push_back(ele);
		}
	}
	list[len][c][s].push_back(id);
}

map<string, vector<int>> inverted_list::get_ele(int len, int c)
{
	return list[len][c];
}

bool inverted_list::exist(int len)
{
	vector<int>::iterator it = find(len_list.begin(), len_list.end(), len);
	if (it == len_list.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void inverted_list::print(int c)
{
	cout << "-----------------------------------------------" << endl;
	for (int i = 0; i < len_list.size(); i++)
	{
		int tmp_num = len_list[i];
		cout << "len = " << len_list[i] << endl;
		for (int j = 0; j < c; j++)
		{
			cout << "c=" << j << endl;
			map<string, vector<int>> tmp = list[len_list[i]][j];
			map<string, vector<int>>::iterator iter;
			iter = tmp.begin();
			while (iter != tmp.end())
			{
				string tmp_str = iter->first;
				vector<int> tmp_list = iter->second;
				cout << tmp_str << endl;
				for (int k = 0; k < tmp_list.size(); k++)
				{
					cout << tmp_list[k] << endl;
				}
				iter++;
			}
		}
	}
	cout << "-----------------------------------------------" << endl;
}

int simJoin::getDataNum() const
{
	return data.size();
}

bool simJoin::getString(int id, string& out) const
{
	if (id < 0 || id >= data.size())
		return false;
	out = data[id];
	return true;
}

bool simJoin::readData(const string& filename)
{
	string str;
	ifstream datafile(filename, ios::in);
	while (getline(datafile, str))
	{
		data.emplace_back(str);
		len_data.push_back(str.length());
	}
	return true;
}

/*
* It should do a similarity join operation betweent the set of strings from a data file
* such that the edit distance between two string is not larger than the given threshold. The
* format of result is a triple of numbers which respectively stand for the two IDs of the pair of strings
* from the data file and the edit distance between the two strings. All results are stored in a vector, sorted based on the IDs of the string from
* the first file and then the IDs of the string from the second file in an ascending order. Return
* an error if the similarity join operation is failed.
*/


int simJoin::edit_distance(string s1, string s2)
{
	int len_s1 = s1.length();
	int len_s2 = s2.length();
	int delta = len_s1 - len_s2;
	vector<vector <int> > dp(len_s2 + 1, vector <int>(len_s1 + 1, ed_threshold + 99));

	for (int i = 0; i <= len_s1; i++) dp[0][i] = i;
	for (int j = 0; j <= len_s2; j++) dp[j][0] = j;

	for (int i = 1; i <= len_s2; i++)
	{
		int start = max(1, i - (ed_threshold - delta) / 2);
		int end = min(len_s1, i + (ed_threshold + delta) / 2);
		bool terminate = true;

		for (int j = start; j <= end; j++)
		{
			int dis1 = dp[i - 1][j] + 1;
			int dis2 = dp[i][j - 1] + 1;
			int dis3 = (s1[j - 1] == s2[i - 1]) ? dp[i - 1][j - 1] : dp[i - 1][j - 1] + 1;

			dp[i][j] = min(dis1, dis2);
			dp[i][j] = min(dp[i][j], dis3);
			if (dp[i][j] <= ed_threshold) terminate = false;
		}
		if (terminate) return ed_threshold + 99;
	}

	return dp[len_s2][len_s1];
};

void simJoin::select_substring(string s, map<string, vector<int>> dict, int str_l, int& cur_pos, int i, set<int>& candidate)
{
	int delta = s.length() - str_l;
	int start_pos = max(cur_pos - (i - 1), cur_pos + delta - (ed_threshold + 1 - i));
	int end_pos = min(cur_pos + (i - 1), cur_pos + delta + (ed_threshold + 1 - i));
	/*cout << "String= " << s << ", start= " << start_pos << ", end= " << end_pos << endl;
	cout << "p " << cur_pos << ",i " << i << ",delta " << delta << ",tao " << ed_threshold << endl;*/
	map<string, vector<int>>::iterator iter;
	iter = dict.begin();
	int cur_len = iter->first.length();
	while (iter != dict.end())
	{
		string tmp_str = iter->first;
		vector<int> tmp_list = iter->second;
		/*cout << "target " << tmp_str << endl;*/
		for (int pos = start_pos; pos <= end_pos; pos++)
		{
			string sub_str = s.substr(pos, cur_len);
			/*cout << sub_str << " ";*/
			if (sub_str == tmp_str)
			{
				/*cout << "(match)" << " ";*/
				for (i = 0; i < tmp_list.size(); i++)
				{
					candidate.insert(tmp_list[i]);
				}
			}
		}
		/*cout << endl;*/
		iter++;
	}
	cur_pos += cur_len;
}

bool simJoin::SimilarityJoin(unsigned threshold, vector< triple<unsigned, unsigned, unsigned> >& results)
{
	ed_threshold = threshold;
	int data_size = getDataNum();
	inverted_list my_list(ed_threshold + 1);

	for (int i = 0; i < data_size; i++)
	{
		string s = data[i];
		int len_s = len_data[i];
		int len_min = min(len_s - ed_threshold, 1);
		for (int tmp_len = len_min; tmp_len <= len_s; tmp_len++)
		{
			if (my_list.exist(tmp_len))
			{
				/*cout << "find" << tmp_len << endl;*/
				set<int> candidates;
				int cur_pos = 0;
				for (int cnt = 0; cnt <= ed_threshold; cnt++)
				{
					map<string, vector<int>> tmp_dict;
					tmp_dict = my_list.get_ele(tmp_len, cnt);
					select_substring(s, tmp_dict, tmp_len, cur_pos, cnt + 1, candidates);
				}

				set<int>::iterator iter;
				iter = candidates.begin();
				while (iter != candidates.end())
				{
					/*cout << "-----------------------------------------------" << endl;
					cout << "matching: " << endl;
					cout << s << " " << data[*iter] << " " << endl;*/
					int dis = edit_distance(s, data[*iter]);
					/*cout << s << " " << data[*iter] << " " << dis << endl;*/
					if (dis <= ed_threshold)
					{
						triple<unsigned, unsigned, unsigned> tmp_ans = { i, *iter, dis };
						results.push_back(tmp_ans);
					}
					iter++;
				}
				/*cout << "-----------------------------------------------" << endl;*/
			}
		}
		int curr_pos = 0;
		int k = len_s - len_s / (ed_threshold + 1) * (ed_threshold + 1);
		int len1 = len_s / (ed_threshold + 1);
		int len2 = ceil(double(len_s) / (double(ed_threshold) + 1));

		for (int n = 0; n < ed_threshold + 1 - k; n++)
		{
			string sub_str = s.substr(curr_pos, len1);
			/*cout << "inseret" << len_s << " " << n << " " << sub_str << " " << i << endl;*/
			my_list.insert_ele(len_s, n, sub_str, i);
			curr_pos += len1;
		}

		for (int n = ed_threshold + 1 - k; n < ed_threshold + 1; n++)
		{
			string sub_str = s.substr(curr_pos, len2);
			/*cout << "inseret" << len_s << " " << n << " " << sub_str << " " << i << endl;*/
			my_list.insert_ele(len_s, n, sub_str, i);
			curr_pos += len2;
		}
		/*my_list.print(ed_threshold + 1);*/
	}

	return true;
}
