#include<functional>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <cmath>
#include <queue>
#include <array>
#include <map>
#include <stack>

using i64 = long long;


struct edge {
	int u, v;
	int type;//01 human,10 car,11 human + car
	int len;
	std::vector<std::pair<int, int>> info;

	edge() : type(0), len(0) {}

	edge(int u, int v, int type, int len) : type(type), len(len) {}

	void show() {
		static char di[4] = {'n', 'w', 'e', 's'};
		std::cout << "From " << u << " to " << v << ":" << std::endl;
		for (auto [dir, ln]: info) {
			std::cout << di[dir] << " " << ln << "m " << std::endl;
		}
	}
};

struct point {
	int id;
	std::string name;
	std::vector<std::string> intro;

	void show() {
		std::cout << name << ":";
		for (auto& str: intro)std::cout << str << ' ';
	}
};

std::vector<point> points;
std::map<std::string, int> names;
std::vector<std::vector<int>> dis, nxt;
std::vector<std::vector<edge>> cur;
std::vector<std::vector<edge>> mat;
int n;
int curType;

void floyd() {
	for (int k = 0; k < n; ++k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (dis[i][j] > dis[i][k] + dis[k][j]) {
					dis[i][j] = dis[i][k] + dis[k][j];
					nxt[i][j] = k;
				}
			}
		}
	}
}

void build(int type) {
	if (curType != type) {
		dis = std::vector(n, std::vector<int>(n, 0x3f3f3f3f));
		nxt = std::vector(n, std::vector<int>(n, -1));
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				auto x = mat[i][j];
				auto [_u, _v, t, w, _] = x;
				if (t & type) {
					if (dis[i][j] > w) {
						cur[i][j] = x;
						dis[i][j] = w;
					}
				}
			}
		}
		curType = type;
	}
}

void sp(int x) {
	build(x);
	floyd();
}

void init() {
	std::cin >> n;
	points.resize(n);

	for (int i = 0; i < n; ++i) {
		points[i].id = i + 1;
		int m;
		std::cin >> points[i].name >> m;
		points[i].intro.resize(m);
		for (int j = 0; j < m; ++j) {
			std::cin >> points[i].intro[j];
		}
		names[points[i].name] = i;
		names[std::to_string(i + 1)] = i;
	}

	dis = std::vector(n, std::vector<int>(n, 0x3f3f3f3f));
	nxt = std::vector(n, std::vector<int>(n, -1));
	cur = std::vector(n, std::vector<edge>(n));

	int m;
	std::cin >> m;
	for (int i = 0; i < m; ++i) {
		int u, v, w, type;
		std::cin >> u >> v >> w >> type;
		u--, v--;

		edge pos(u, v, w, type);

		int t;
		std::cin >> t;
		for (int j = 0; j < t; ++j) {
			int dir; //w - western 1,e - eastern 2, s - south 3, n - north 0
			int len;
			std::cin >> dir >> len;
			pos.info.emplace_back(dir, len);
		}

		//generate another edge
		auto neg = pos;
		std::swap(neg.u, neg.v);
		std::reverse(neg.info.begin(), neg.info.end());
		for (auto& x: neg.info) {
			x.first = x.first ^ 3;
		}
		mat[u][v] = pos;
		mat[v][w] = neg;
	}

	build(3);
}

void qrySpot() {
	std::cout << "Enter the name or id you want to search" << std::endl;
	std::string x;
	std::cin >> x;
	if (names.count(x)) {
		auto id = names[x];
		points[id].show();
		std::cout << std::endl;
	} else {
		std::cout << "Spot not found" << std::endl;
	}
}

void shortestPath() {
	std::cout << "Enter the type of roads you want to query" << std::endl;
	int x;
	std::cin >> x;

	sp(x);
	std::cout << "Enter the two spots' name or id you want to query" << std::endl;
	std::string s, r;
	std::cin >> s >> r;
	int u = names[s], v = names[r];
	if (dis[u][v] == 0x3f3f3f3f) {
		std::cout << "The minimum distance is" << dis[u][v] << '\n';
	} else {
		std::cout << "The minimum distance is" << dis[u][v] << '\n';
		std::cout << "The routine distance is" << dis[u][v] << '\n';
		int now = nxt[u][v];
		while (now != u) {
			std::cout << now << ' ' << nxt[now][u] << '\n';
			now = nxt[now][u];
		}
	}
}


void tarjan() {
	build(3);
	std::stack<int> s;
	std::vector<int> dfn(n, -1), low(n, -1), inStack(n);
	int cnt = 0;
	std::vector<bool> cut(n, false);
	std::function<void(int, int)> dfs = [&](int u, int p) {
		dfn[u] = cnt++;
		inStack[u] = true;
		s.push(u);
		int child = 0;
		for (int v = 0; v < n; ++v) {
			if (dis[u][v] == 0x3f3f3f3f)continue;
			if (v == p)continue;
			if (dfn[v] == -1) {
				dfs(v, u);
				child++;
				low[u] = std::min(low[u], low[v]);
				if (low[v] >= dfn[u]) {
					cut[u] = true;
				}
			} else low[u] = std::min(low[u], dfn[v]);
		}
		if (child == 1 && u == p)cut[u] = false;
	};
	for (int i = 0; i < n; ++i) {
		if (dfn[i] == -1)dfs(i, i);
	}
	int num = 0;
	for (int i = 0; i < n; ++i)if (cut[i])num++;
	if (num) {
		std::cout << "There " << (num == 1 ? "is" : "ares") << num << " cut point" << (num == 1 ? ":" : "s:") << std::endl;
		for (int i = 0; i < n; ++i) {
			if (cut[i])std::cout << i + 1 << std::endl;
		}
	} else {
		std::cout << "There is no cut point!" << std::endl;
	}
}

void showAllPath() {
	std::cout << "Enter the two spots' name or id you want to query" << std::endl;
	std::string s, r;
	std::cin >> s >> r;
	int begin = names[s], end = names[r];
	int cnt = 0;
	std::vector<int> path;
	std::vector<int> vis(n, false);
	std::vector<std::vector<int>> paths;
	std::function<void(int)> dfs = [&](int u) {
		if (u == end) {
			cnt++;
			paths.push_back(path);
			return;
		}
		vis[u] = true;
		for (int v = 0; v < n; ++v) {
			if (mat[u][v].type || vis[v])continue;
			path.push_back(v);
			dfs(v);
			path.pop_back();
		}
		vis[u] = false;
	};
	if (cnt) {
		std::cout << "There " << (cnt == 1 ? "is" : "ares") << cnt << " path" << (cnt == 1 ? ":" : "s:") << std::endl;
		for (auto& x: paths) {
			int m = x.size();
			for (int i = 1; i < m; ++i) {
				mat[x[i - 1]][x[i]].show();
			}
		}
	} else {
		std::cout << "No path!" << std::endl;
	}
}

void multiPointSP() {
	int m;
	std::cin >> m;
	std::vector<int> a(m);
	for (int i = 0; i < m; ++i) {
		std::cin >> a[i];
	}
	int ans = 0x3f3f3f3f;
	int sum_dis = 0;
	bool ok = true;
	std::vector<int> routine;
	for (int i = 1; i < m; ++i) {
		if (dis[a[i - 1]][a[i]] == 0x3f3f3f3f) {
			ok = false;
		} else {
			sum_dis += dis[a[i - 1]][a[i]];
		}
	}
	if (ok) {
		for (int i = 1; i < m; ++i) {
			int now = a[i - 1];
			while (now != a[i]) {
				routine.push_back(now);
				now = nxt[now][a[i]];
			}
		}
	} else {
		std::cout << "No path!" << std::endl;
	}
};

void modify() {
	std::string s, r;
	std::cin >> s >> r;
	int u = names[s], v = names[r];
	int t, len;
	std::cin >> t >> len;
	edge now(u, v, t, len);
	int m;
	std::cin >> m;
	for (int i = 0; i < m; ++i) {
		int d, l;
		std::cin >> d >> l;
		now.info.emplace_back(d, l);
	}
	mat[u][v] = now;
	std::cout << "Succeed" << std::endl;
}

void menu() {
	std::cout << "-------------------------------------------------------------------" << std::endl;
	std::cout << "|       1.Search spot                                             |" << std::endl;
	std::cout << "|       2.Query shortest path between two spots                   |" << std::endl;
	std::cout << "|       3.Query cut point                                         |" << std::endl;
	std::cout << "|       4.Query all path between two spots                        |" << std::endl;
	std::cout << "|       5.Query shortest path between several spots               |" << std::endl;
	std::cout << "|       6.Modify road information                                 |" << std::endl;
	std::cout << "-------------------------------------------------------------------" << std::endl;
}

int main() {
	init();
	menu();
}