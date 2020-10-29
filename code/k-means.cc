#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <float.h>
#include <string>

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

double calcDist(std::vector<std::vector<double>> g /* 重心の配列 */, double data[200][2] /* データ */, int gi /* 重心のインデックス */, int i /* データのインデックス */)
{
	// trace c("calcDist");
	return std::sqrt(std::pow(data[i][0] - g[gi][0], 2) + std::pow(data[i][1] - g[gi][1], 2));
}

bool checkG(std::vector<std::vector<double>> g, std::vector<std::vector<double>> g_old)
{
	// trace cg("checkG");
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		if (g[i] == g_old[i])
		{
			count++;
		}
	}
	return count == 3;
}

void showG(std::vector<std::vector<double>> g)
{
	std::cout << "現在の重心" << std::endl;
	for (int i = 0; i < g.size(); i++)
	{
		std::cout << g[i][0] << " " << g[i][1] << std::endl;
	}
}

int main(int argc, char *argv[])
{
	// trace m("main");
	FILE *fp_in, *fp_out;
	int input;
	int count = 0;
	double data[200][2]; // the number of data is 200
	int label[200];		 // kのインデックスを格納
	int i = 0;
	int k; // クラスタ数

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

	std::cout << "クラスタ数を入力してください: ";
	std::cin >> k;

	//k-means//
	// ランダムにk個の点を初期重心として指定する
	std::vector<std::vector<double>>
		g(k, std::vector<double>(2));								   // 現在の重心の座標の配列
	std::vector<std::vector<double>> g_old(k, std::vector<double>(2)); // 前回の重心の座標の配列
	for (i = 0; i < k; i++)
	{
		int idx = rand() % 200;
		g[i][0] = data[idx][0];
		g[i][1] = data[idx][1];
	}
	while (!checkG(g, g_old)) // 重心位置が変化していれば繰り返す
	{
		// trace w("while roop");
		// showG(g);

		// 重心位置を元にラベルを更新する
		for (int l = 0; l < 200; l++) // 200個のデータについて
		{
			double min_dist = DBL_MAX;	// distの最小値を格納する変数。初期は最大値を設定しておく
			int min_dist_label;			// distが最小の時のクラスタリングラベル（kのインデックス）を格納する変数
			for (int j = 0; j < k; j++) // k個の重心それぞれについて
			{
				double dist = calcDist(g, data, j, l);
				if (dist < min_dist)
				{
					min_dist = dist;
					min_dist_label = j;
				}
			}
			label[l] = min_dist_label;
		}
		// 更新されたラベルを元に重心位置を更新する
		for (int j = 0; j < k; j++) // k個の重心それぞれについて
		{
			// 現在の重心の値を保存する
			g_old[j][0] = g[j][0];
			g_old[j][1] = g[j][1];
			// 新しい重心の値を計算
			double count = 0;
			double sum_1 = 0;
			double sum_2 = 0;
			for (int l = 0; l < 200; l++) // 200個のデータについて
			{
				if (label[l] == j) // ラベルがjであるなら
				{
					count++;
					sum_1 += data[l][0];
					sum_2 += data[l][1];
				}
			}
			g[j][0] = sum_1 / count;
			g[j][1] = sum_2 / count;
		}
	}

	///////////

	//Output//
	fp_out = fopen(argv[2], "w");
	if (fp_out == NULL)
	{
		printf("fail: cannot open the output-file. Change the name of output-file.  \n");
		return -1;
	}

	for (i = 0; i < 200; i++)
	{
		fprintf(fp_out, "%lf,%lf,%d\n", data[i][0], data[i][1], label[i]);
	}
	return 0;
}
