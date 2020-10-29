#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <float.h>
#include <string>

// デバッグ用
class trace
{
private:
	std::string name;

public:
	trace(const std::string &nm)
	{												// 文字列引数を持つコンストラクタ
		name = nm;									// 受け取った文字列を記憶
		std::cerr << name << " begin" << std::endl; // ○○○ begin と表示
	}
	~trace()
	{											  // デストラクタ
		std::cerr << name << " end" << std::endl; // ○○○ end と表示
	}
};

class idx_distance
{
public:
	int idx;
	double distance;
	idx_distance(int i, double d)
	{
		idx = i;
		distance = d;
	}
};

class by_distance
{
public:
	bool operator()(const idx_distance &a, const idx_distance &b) const
	{
		return a.distance < b.distance;
	}
};

double d(int idx_a, int idx_b, double data[200][2])
{
	// trace c("d");
	return std::sqrt(std::pow(data[idx_a][0] - data[idx_b][0], 2) + std::pow(data[idx_a][1] - data[idx_b][1], 2));
}

std::vector<int> N_k(int idx, int k, double data[200][2])
{
	// trace k_distance("N_k");
	std::vector<idx_distance> idx_distance_list;
	std::vector<int> N_k;
	for (int i = 0; i < 200; i++)
	{
		if (i != idx) // 相手が自分自身の場合はスキップ
		{
			idx_distance point(i, d(idx, i, data));
			idx_distance_list.push_back(point);
		}
	}
	sort(idx_distance_list.begin(), idx_distance_list.end(), by_distance());

	double border_distance = idx_distance_list[k - 1].distance;
	int i = 0;
	while (idx_distance_list[i].distance <= border_distance)
	{
		N_k.push_back(idx_distance_list[i].idx);
		i++;
	}
	return N_k;
}

double reach_dist(int idx_a, int idx_b, double data[200][2], std::vector<double> distance_list)
{
	double ab_distance = d(idx_a, idx_b, data);
	double k_distance = distance_list[idx_b];
	return std::max(ab_distance, k_distance);
}

double lrd(int idx, double data[200][2], std::vector<double> distance_list, std::vector<std::vector<int>> N_list)
{
	int N_size = N_list[idx].size();
	double reach_dist_sum = 0;
	for (int i = 0; i < N_size; i++)
	{
		reach_dist_sum += reach_dist(idx, N_list[idx][i], data, distance_list);
	}
	return N_size / reach_dist_sum;
}

double lof(int idx, double data[200][2], std::vector<double> distance_list, std::vector<std::vector<int>> N_list, std::vector<double> lrd_list)
{
	double a_lrd = lrd_list[idx];
	double b_lrd_sum = 0;
	int N_size = N_list[idx].size();
	for (int i = 0; i < N_size; i++)
	{
		int b_idx = N_list[idx][i];
		b_lrd_sum += lrd_list[b_idx];
	}
	return (b_lrd_sum / N_size) / a_lrd;
}

int main(int argc, char *argv[])
{
	// trace m("main");
	FILE *fp_in, *fp_out;
	int input;
	int count = 0;
	double data[200][2]; // the number of data is 200
	int i = 0;
	int k;								  // k近傍の値を格納する変数
	std::vector<std::vector<int>> N_list; // 各データのN_kを格納する配列
	std::vector<double> distance_list;	  // 各データのk_distanceを格納する配列
	std::vector<double> lrd_list;		  // 各データのlrd_kを格納する配列
	std::vector<double> lof_list;		  // 各データのlofを格納する配列

	//Input//
	fp_in = fopen(argv[1], "r");
	if (fp_in == NULL)
	{
		printf("fail: cannot open the input-file. Change the name of input-file. \n");
		return -1;
	}
	while ((input = fscanf(fp_in, "%lf,%lf", &data[count][0], &data[count][1])) != EOF) // データの読み込み
	{
		count++;
	}

	// k近傍の値を設定する
	std::cout << "k近傍の値を入力してください: ";
	std::cin >> k;

	//lof//
	for (int i = 0; i < 200; i++)
	{
		N_list.push_back(N_k(i, k, data));					   // 各データについてN_kを計算し、N_listに格納
		distance_list.push_back(d(i, N_list[i].back(), data)); // 各データについてk_distanceを計算し、distance_listに格納
	}

	// 各データについてlrd_kを計算し、lrd_listに格納
	for (int i = 0; i < 200; i++)
	{
		lrd_list.push_back(lrd(i, data, distance_list, N_list));
	}

	// 各データについてLOFを計算し、lof_listに格納
	for (int i = 0; i < 200; i++)
	{
		lof_list.push_back(lof(i, data, distance_list, N_list, lrd_list));
	}
	///////////

	//Output//
	fp_out = fopen(argv[2], "w");
	if (fp_out == NULL)
	{
		printf("fail: cannot open the output-file. Change the name of output-file.  \n");
		return -1;
	}
	// lof_value_listを出力
	for (i = 0; i < 200; i++)
	{
		fprintf(fp_out, "%lf,%lf,%lf\n", data[i][0], data[i][1], lof_list[i]);
	}
	return 0;
}
