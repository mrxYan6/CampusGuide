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
	int type;//01 human,10 car,11 human + car
	int len;
	std::vector<std::pair<int, int>> info;

	edge() {}

	edge(int type, int len) : type(type), len(len) {}
};

struct point {
	int id;
	std::string name;
	std::vector<std::string> intro;
};

std::vector<point> points;
std::map<std::string, int> names;
std::vector<std::vector<int>> dis, nxt;
std::vector<std::vector<edge>> cur;
std::vector<std::vector<std::vector<edge>>> mat;
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
				for (auto& x: mat[i][j]) {
					auto [t, w, _] = x;
					if (t & type) {
						if (dis[i][j] > w) {
							cur[i][j] = x;
							dis[i][j] = w;
						}
					}
				}
			}
		}
		curType = type;
	}
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
	int m;
	std::cin >> m;
	for (int i = 0; i < m; ++i) {
		int u, v, w, type;
		int t;
		std::cin >> u >> v >> w >> type;
		u--, v--;
		std::cin >> t;
		for (int j = 0; j < t; ++j) {

		}
		mat[u][v].emplace_back(w, type);
		mat[v][w].emplace_back(w, type);

//		points[u].e.emplace_back(v, w, type);
//		points[v].e.emplace_back(u, w, type);
	}

	build(3);

	floyd();
}

void qrySpot() {
	std::cout << "Enter the name or id you want to search" << std::endl;
	std::string x;
	std::cin >> x;
	if (names.count(x)) {
		auto id = names[x];
		std::cout << points[id].name << ":";
		for (auto& str: points[id].intro)std::cout << str << ' ';
		std::cout << std::endl;
	} else {
		std::cout << "Spot not found" << std::endl;
	}
}

void shortestPath() {
	std::cout << "Enter the two spots' name or id you want to query" << std::endl;
	std::string s, r;
	std::cin >> s >> r;
	int u = names[s], v = names[r];
	if (dis[u][v] == 0x3f3f3f3f) {
		std::cout << "The minimum distance is" << dis[u][v] << '\n';
	} else {
		std::cout << "The minimum distance is" << dis[u][v] << '\n';
		std::cout << "The routine distance is" << dis[u][v] << '\n';
		int cur = nxt[u][v];
		while (cur != u) {
			std::cout << cur << ' ' << nxt[cur][u] << '\n';
			cur = nxt[cur][u];
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
		for (auto [v, _1, _2]: points[u].e) {
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
	std::vector<int> cur;
	std::vector<int> vis(n, false);
	std::vector<std::vector<int>> paths;
	std::function<void(int)> dfs = [&](int u) {
		if (u == end) {
			cnt++;
			paths.push_back(cur);
			return;
		}
		vis[u] = true;
		for (auto [v, t, w]: points[u].e) {
			if (vis[v])continue;
			cur.push_back(v);
			dfs(v);
			cur.pop_back();
		}
		vis[u] = false;
	};
	if (cnt) {
		std::cout << "There " << (cnt == 1 ? "is" : "ares") << cnt << " path" << (cnt == 1 ? ":" : "s:") << std::endl;
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
	int cur = 0;
	bool ok = true;
	std::vector<int> routine;
	for (int i = 1; i < m; ++i) {
		if (dis[a[i - 1]][a[i]] == 0x3f3f3f3f) {
			ok = false;
		} else {
			cur += dis[a[i - 1]][a[i]];
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
	int begin = names[s], end = names[r];
	int wp, tp;
	std::cin >> wp >> tp;
	bool find = false;
	for (auto& [v, t, w]: points[begin].e) {
		if (v == end) {
			t = tp;
			w = wp;
			find = true;
			break;
		}
	}
	for (auto& [v, t, w]: points[end].e) {
		if (v == begin) {
			t = tp;
			w = wp;
			find = true;
			break;
		}
	}
	if (!find) {
		points[begin].e.emplace_back(end, tp, wp);
		points[end].e.emplace_back(begin, tp, wp);
	}
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