#include "simJoin.h"

void inverted_list::insert_ele(int len, int c, string s, int id)
{
	// vector<int>::iterator it = find(len_list.begin(), len_list.end(), len);
	map<int, vector<map<string, vector<int>>>>::iterator it;
	it = list.find(len);
	if (it == list.end())
	{
		// len_list.push_back(len);
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
	map<int, vector<map<string, vector<int>>>>::iterator it;
	it = list.find(len);
	if (it == list.end())
	{
		return false;
	}
	else
	{
		return true;
	}
	
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


int simJoin::edit_distance(string s1, string s2, int threshold)
{
	int len_s1 = s1.length();
	int len_s2 = s2.length();
	if (len_s1 == 0 || len_s2 == 0)
	{
	    return abs(len_s1 - len_s2);
	}
	int delta = len_s1 - len_s2;
	int dp[len_s2 + 1][len_s1 + 1];
	for (int i = 0; i <= len_s1; i++) dp[0][i] = i;
	for (int j = 0; j <= len_s2; j++) dp[j][0] = j;
	dp[len_s2][len_s1] = threshold + 99;

	for (int i = 1; i <= len_s2; i++)
	{
		int start = max(1, i - (threshold - delta) / 2);
		int end = min(len_s1, i + (threshold + delta) / 2);
		bool terminate = true;

		for (int j = start; j <= end; j++)
		{
			int dis1 = dp[i - 1][j] + 1;
			if (j == i + (threshold + delta) / 2) dis1 = threshold + 99;
			int dis2 = dp[i][j - 1] + 1;
			if (j == i - (threshold - delta) / 2) dis2 = threshold + 99;
			int dis3 = (s1[j - 1] == s2[i - 1]) ? dp[i - 1][j - 1] : dp[i - 1][j - 1] + 1;

			dp[i][j] = min(dis1, dis2);
			dp[i][j] = min(dp[i][j], dis3);
			if (dp[i][j] <= threshold) terminate = false;
		}
		if (terminate) return threshold + 99;
	}

	return dp[len_s2][len_s1];
};

int simJoin::extension_based_verrification(string s11, string s12, string s21, string s22, int i)
{
	int threshold1 = i - 1;
	int threshold2 = ed_threshold + 1 - i;
	int d1 = edit_distance(s11, s21, threshold1);
	if (d1 <= threshold1)
	{
		int d2 = edit_distance(s12, s22, threshold2);
		{
			if (d2 <= threshold2)
			{
				return d1 + d2;
			}
		}
	}
	return ed_threshold + 99;
};

void simJoin::select_substring(int s_id, map<string, vector<int>> dict, int str_l, int& cur_pos, int i, set<int>& candidate, vector< triple<unsigned, unsigned, unsigned> >& results)
{
	string s = data[s_id];
	int delta = s.length() - str_l;
	int start_pos = max(cur_pos - (i - 1), cur_pos + delta - (ed_threshold + 1 - i));
	int end_pos = min(cur_pos + (i - 1), cur_pos + delta + (ed_threshold + 1 - i));
	//cout << "String= " << s << ", start= " << start_pos << ", end= " << end_pos << endl;
	//cout << "p " << cur_pos << ",i " << i << ",delta " << delta << ",tao " << ed_threshold << endl;
	
	int k = str_l - str_l / (ed_threshold + 1) * (ed_threshold + 1);
	int cur_len = 0;
	if (i <= ed_threshold + 1 - k)
	{
		cur_len = str_l / (ed_threshold + 1);
	}
	else
	{
		cur_len = ceil(double(str_l) / (double(ed_threshold) + 1));
	}
	
	for (int pos = start_pos; pos <= end_pos; pos++)
	{
		string sub_str = s.substr(pos, cur_len);
		//cout << sub_str << " ";
		map<string, vector<int>>::iterator iter;
		iter = dict.find(sub_str);
		if (iter != dict.end())
		{
			//cout << "(match)" << " ";
			for (int j = 0; j < dict[sub_str].size(); j++)
			{
				set<int>::iterator it;
				it = candidate.find(dict[sub_str][j]);
				if (it == candidate.end())
				{
					int r = dict[sub_str][j];	
					string s11 = s.substr(0, pos);
					string s12 = s.substr(pos + cur_len, s.length() - pos - cur_len);
					string s21 = data[r].substr(0, cur_pos);
					string s22 = data[r].substr(cur_pos + cur_len, s.length() - cur_pos - cur_len);
					int ed_distance = extension_based_verrification(s11, s12, s21, s22, i);
					if (ed_distance <= ed_threshold)
					{
						candidate.insert(r);
						triple<unsigned, unsigned, unsigned> tmp_ans = { s_id, r, ed_distance };
						results.push_back(tmp_ans);
					}
				}
			}
		}
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
		if (i%100==0)
		{
			cout << i<<endl;
		}
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
					select_substring(i, tmp_dict, tmp_len, cur_pos, cnt + 1, candidates, results);
				}
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
