#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <map>

using namespace std;

const int INF = 0x3f3f3f3f;
const int teamNum = 5, matchNum = 7; // 比赛节点数
vector<vector<int>> capacity;
vector<vector<int>> edge; // edge[i][j] 表示 i -> j 的边
vector<vector<int>> match(teamNum, vector<int>(teamNum));
vector<pair<pair<int, int>, int>> matchNode;

struct Team {
    int win;
    int loss;
    int left;
    string name;
} teamInfo[teamNum];

void show() {
    for (int i = 0; i < teamNum; i++) {
        cout << teamInfo[i].name << ": " << teamInfo[i].win << " " << teamInfo[i].loss << " " << teamInfo[i].left << endl;
    }
    for (int i = 0; i < teamNum; i++) {
        for (int j = 0; j < teamNum; j++) cout << match[i][j] << " ";
        cout << endl;
    }
    for (int i = 0; i < matchNum; i++) {
        cout << matchNode[i].first.first << "-" << matchNode[i].first.second << ": " << matchNode[i].second << endl;
    }
}

void initiate() {
    string fileName = "Lab_6.txt";
    fstream infile(fileName);
    if (!infile) {
        cout << "file not exists" << endl;
        return;
    }
    for (int i = 0; i < teamNum; i++) {
        infile >> teamInfo[i].win >> teamInfo[i].loss >> teamInfo[i].left >> teamInfo[i].name;
    }
    for (int i = 0; i < teamNum; i++) {
        for (int j = 0; j < teamNum; j++) infile >> match[i][j];
    }
    for (int i = 0; i < teamNum; i++) {
        for (int j = i + 1; j < teamNum; j++) {
            matchNode.emplace_back(make_pair(i, j), match[i][j]);
        }
    }
//    show();
    cout << "data initiated" << endl;
    infile.close();
}

void addEdge(int u, int v, int cap) {
    capacity[u][v] = cap;
    edge[u].push_back(v);
    edge[v].push_back(u); // 反向边
}

int bfs(int source, int sink, vector<int>& parent) {
    fill(parent.begin(), parent.end(), -1);
    parent[source] = source;
    queue<pair<int, int>> q;
    q.push({source, INF});
    while (!q.empty()) {
        int cur = q.front().first;
        int flow = q.front().second;
        q.pop();
        for (int next : edge[cur]) {
            if (parent[next] == -1 && capacity[cur][next]) {
                parent[next] = cur;
                int new_flow = min(flow, capacity[cur][next]);
                if (next == sink)
                    return new_flow;
                q.push({next, new_flow});
            }
        }
    }
    return 0;
}

int maxFlow(int source, int sink) {
    int flow = 0;
    vector<int> parent(matchNum + teamNum + 2);
    int new_flow;
    while (new_flow = bfs(source, sink, parent)) {
        flow += new_flow;
        int cur = sink;
        while (cur != source) {
            int prev = parent[cur];
            capacity[prev][cur] -= new_flow;
            capacity[cur][prev] += new_flow;
            cur = prev;
        }
    }
    return flow;
}

void buildNetwork(int team) {
    int totalNodes = matchNum + teamNum + 2;
    capacity.resize(totalNodes, vector<int>(totalNodes, 0));
    edge.resize(totalNodes, vector<int>());
    int source = 0;
    int sink = totalNodes - 1;
    int matchIndex = 1;
    int teamIndex = matchNum + 1;
    for (auto match : matchNode) {
        if (match.first.first != team && match.first.second != team) {
            addEdge(source, matchIndex, match.second);
            addEdge(matchIndex, teamIndex + match.first.first, INF);
            addEdge(matchIndex, teamIndex + match.first.second, INF);
            matchIndex++;
        }
    }
    int maxPossibleWins = teamInfo[team].win + teamInfo[team].left;
    for (int i = 0; i < teamNum; i++) {
        if (i != team) {
            int capacityToSink = maxPossibleWins - teamInfo[i].win;
            if (capacityToSink < 0) capacityToSink = 0;
            addEdge(teamIndex + i, sink, capacityToSink);
        }
    }
}

void solve() {
    initiate();
    for (int i = 0; i < teamNum; i++) {
        buildNetwork(i);//根据队伍i的约束建立流网络
        int source = 0;
        int sink = matchNum + teamNum + 1;
        int totalGames = 0;
        for (auto match : matchNode) {
            if (match.first.first != i && match.first.second != i) {
                totalGames += match.second;
            }
        }
        int max_flow = maxFlow(source, sink);
        if (max_flow == totalGames) {
            cout << teamInfo[i].name << " is not eliminated." << endl;
        } else {
            cout << teamInfo[i].name << " is eliminated." << endl;
        }
    }
}

int main() {
    solve();
    return 0;
}
