#ifndef BKZALGORITHMS_H
#define BKZALGORITHMS_H

#include <algorithm>
#include <deque>
#include <queue>
#include <sstream>
#include <stack>
#include <vector>

#include <QDebug>

using namespace std;

namespace bkz {
class Algo {
  public:
	// vector输出
	template <typename T>
	static string outputVt(const vector<T> &vt);
	template <typename T>
	static string outputVt2(const vector<vector<T>> &vt);

	/*  前缀和部分
         *  o(1)求区间和
         *  这里求值范围是后移一位的，也就是getsum(vt,1,2)实际求的是[0,1]的区间和，二维也类似
         **/

	template <typename T> // 一维前缀和预处理
	static vector<T> prefixSum(const vector<T> &vt);
	template <typename T> // 对预处理数组求和
	static T getsum(const vector<T> &vt, const int &l, const int &r);

	template <typename T>											   // 二维前缀和预处理
	static vector<vector<T>> prefixSum2d(const vector<vector<T>> &vt); // 二维前缀和预处理
	template <typename T>											   // 二维求和，(x1,y1)左上角，另一个右下
	static T getsum2d(vector<vector<T>> &vt, const int &x1, const int &y1, const int &x2, const int &y2);

	/*  一维单调队列
         *  对于每个位置，取往前col个值（包括自身），在此范围内的最值
         *  loc=0时得到数值，否则得到位置
         *  mode=0时取最小值，其他情况最大值
         *  考虑到其他需求，将长度从n扩展到n+col-1防止置其他取范围方法越界
         **/
	template <typename T> // 使用vector下src和res如果传入同一vector比较麻烦
	static vector<T> monoque(const vector<T> &src, const int &cols, const int &mode = 0, const int &loc = 0);

	/*  二维单调队列
         *  对于每个位置，取row行col列（包括自身），在此范围内的最值
         *  带loc的函数是去位置，另一个数值
         *  mode=0时取最小值，其他情况最大值
         *  考虑到其他需求，将(n,m)扩展到(n+row-1,m+col-1)，防止其他取范围方法越界
         **/

	template <typename T>
	static vector<vector<pair<int, int>>> monoque2d_loc(const vector<vector<T>> &src, const int &rows, const int &cols, const int &mode = 0);

	template <typename T>
	static vector<vector<T>> monoque2d(const vector<vector<T>> &src, const int &rows, const int &cols, const int &mode = 0);

	// 单调栈、st表、树状数组、线段树待补充
};
} // namespace bkz

namespace bkz {
template <typename T>
string Algo::outputVt(const vector<T> &vt) {
	stringstream res;
	for (int i = 0; i < vt.size(); i++) {
		res << vt[i] << ' ';
	}
	return res.str();
}
template <typename T>
string Algo::outputVt2(const vector<vector<T>> &vt) {
	stringstream res;
	if (vt.empty()) {
		return res.str();
	}
	for (int i = 0; i < vt.size(); i++) {
		for (int j = 0; j < vt[i].size(); j++) {
			res << vt[i][j] << ' ';
		}
		res << '\n';
	}
	return res.str();
}

template <typename T>
vector<T> Algo::prefixSum(const vector<T> &vt) {
	int n = vt.size();
	T sum = 0;
	vector<T> res;
	for (int i = 0; i < n; i++) {
		sum += vt[i];
		res.push_back(sum);
	}
	return res;
}

template <typename T>
T Algo::getsum(const vector<T> &vt, const int &l, const int &r) {
	if (vt.empty()) return 0;
	int lt = l < 0 ? 0 : l;
	int rt = r >= vt.size() ? vt.size() - 1 : r;
	if (lt == 0) return vt[rt];
	return vt[rt] - vt[lt - 1];
}

template <typename T>
vector<vector<T>> Algo::prefixSum2d(const vector<vector<T>> &vt) {
	vector<vector<T>> res = vt;
	int n = vt.size(), m = vt[0].size();
	for (int i = 1; i < n; i++) {
		res[i][0] += res[i - 1][0];
	}
	for (int j = 1; j < m; j++) {
		res[0][j] += res[0][j - 1];
	}
	for (int i = 1; i < n; i++) {
		for (int j = 1; j < m; j++) {
			res[i][j] = res[i][j] + res[i - 1][j] + res[i][j - 1] - res[i - 1][j - 1];
		}
	}
	return res;
}

template <typename T>
T Algo::getsum2d(vector<vector<T>> &vt, const int &x1, const int &y1, const int &x2, const int &y2) {
	if (vt.empty() or vt[0].empty()) return 0;
	int nx1 = x1 < 0 ? 0 : x1;
	int ny1 = y1 < 0 ? 0 : y1;
	int nx2 = x2 >= vt.size() ? vt.size() - 1 : x2;
	int ny2 = y2 >= vt[0].size() ? vt[0].size() - 1 : y2;

	if (nx1 == 0 and ny1 == 0) {
		return vt[nx2][ny2];
	} else if (nx1 == 0) {
		return vt[nx2][ny2] - vt[nx2][ny1 - 1];
	} else if (ny1 == 0) {
		return vt[nx2][ny2] - vt[nx1 - 1][ny2];
	}
	return vt[nx2][ny2] - vt[x1 - 1][ny2] - vt[nx2][y1 - 1] + vt[x1 - 1][y1 - 1];
}

template <typename T>
vector<T> Algo::monoque(const vector<T> &src, const int &cols, const int &mode, const int &loc) {
	deque<T> q;
	int n = src.size();
	vector<T> res;

	for (int i = 0; i < n + cols - 1; i++) {
		while (!q.empty() and q.front() + cols <= i) {
			q.pop_front();
		}
		if (i < n) {
			if (mode) { // 最大
				while (!q.empty() and src[q.back()] <= src[i]) {
					q.pop_back();
				}
			} else {
				while (!q.empty() and src[q.back()] >= src[i]) {
					q.pop_back();
				}
			}
			q.push_back(i);
		}
		if (loc) res.push_back(q.front());
		else
			res.push_back(src[q.front()]);
	}
	return res;
}

template <typename T>
vector<vector<T>> Algo::monoque2d(const vector<vector<T>> &src, const int &rows, const int &cols, const int &mode) {
	if (src.empty() or src[0].empty()) {
		return src;
	}
	deque<T> q;
	int n = src.size(), m = src[0].size();
	vector<vector<T>> res(n + rows - 1);

	// 这里每一列可以多线程
	for (int j = 0; j < m; j++) {
		q.clear();
		for (int i = 0; i < n + rows - 1; i++) {
			while (!q.empty() and q.front() + rows <= i) {
				q.pop_front();
			}
			if (i < n) {
				if (mode) {
					while (!q.empty() and src[q.back()][j] <= src[i][j]) {
						q.pop_back();
					}
				} else {
					while (!q.empty() and src[q.back()][j] >= src[i][j]) {
						q.pop_back();
					}
				}
			}
			q.push_back(i);
			res[i].push_back(src[q.front()][j]);
			// qDebug() << j << ":?:" << Algo::outputVt(res[i]) << '\n';
		}
	}

	// 这里也可以多线程
	for (int i = 0; i < n + rows - 1; i++) {
		res[i] = monoque(res[i], cols, mode);
	}
	return res;
}

template <typename T>
vector<vector<pair<int, int>>> Algo::monoque2d_loc(const vector<vector<T>> &src, const int &rows, const int &cols, const int &mode) {
	if (src.empty() or src[0].empty()) {
		vector<vector<pair<int, int>>> empty;
		return empty;
	}
	deque<T> q;
	int n = src.size(), m = src[0].size();
	vector<vector<T>> res(n + rows - 1), loc(n + rows - 1);

	for (int j = 0; j < m; j++) {
		q.clear();
		for (int i = 0; i < n + rows - 1; i++) {
			while (!q.empty() and q.front() + rows <= i) {
				q.pop_front();
			}
			if (i < n) {
				if (mode) {
					while (!q.empty() and src[q.back()][j] <= src[i][j]) {
						q.pop_back();
					}
				} else {
					while (!q.empty() and src[q.back()][j] >= src[i][j]) {
						q.pop_back();
					}
				}
			}
			q.push_back(i);
			res[i].push_back(src[q.front()][j]);
			loc[i].push_back(q.front());
		}
	}
	for (int i = 0; i < n + rows - 1; i++) {
		res[i] = monoque(res[i], cols, mode, 1);
	}

	vector<vector<pair<int, int>>> res2;
	res2.resize(n + rows - 1, vector<pair<int, int>>(m + cols - 1));
	for (int i = 0; i < n + rows - 1; i++) {
		for (int j = 0; j < m + cols - 1; j++) {
			res2[i][j] = {loc[i][res[i][j]], res[i][j]};
		}
	}
	return res2;
}
} // namespace bkz

#endif // BKZALGORITHMS_H
