#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <map>

using namespace std;

const int INF = 0x3f3f3f3f;
const int teamNum = 5, matchNum = 7;//队伍数与比赛节点数
vector<vector<int>> capacity;
vector<vector<int>> edge; // edge[i][j] 表示节点 i -> j 的边
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
        cout << teamInfo[i].name << ": " << teamInfo[i].win << " " << teamInfo[i].loss << " " << teamInfo[i].left
             << endl;
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
    capacity[u][v] = cap;//设置边容量
    edge[u].push_back(v);//添加边
    edge[v].push_back(u);//反向边
}

//寻找增广路径
int BFS(int source, int sink, vector<int> &parent) {
    fill(parent.begin(), parent.end(), -1);
    parent[source] = source;
    queue<pair<int, int>> q;
    q.push({source, INF});
    while (!q.empty()) {
        int cur = q.front().first;
        int flow = q.front().second;
        q.pop();
        //遍历当前能够到达的节点
        for (int next: edge[cur]) {
            if (parent[next] == -1 && capacity[cur][next]) {
                parent[next] = cur;
                //取路径中较小的流量
                int new_flow = min(flow, capacity[cur][next]);
                //到达汇点则说明找到了一条增广路径
                if (next == sink)return new_flow;
                q.push({next, new_flow});
            }
        }
    }
    return 0;
}

//使用BFS寻找增广路径即Edmonds-Karp算法
int maxFlow(int source, int sink) {
    int flow = 0;
    vector<int> parent(matchNum + teamNum + 2);//记录增广路径
    int newFlow;
    while (newFlow = BFS(source, sink, parent)) {
        flow += newFlow;
        int cur = sink;
        //遍历路径，修改边容量
        while (cur != source) {
            int prev = parent[cur];
            capacity[prev][cur] -= newFlow;//正向边减少
            capacity[cur][prev] += newFlow;//反向边增加
            cur = prev;
        }
    }
    return flow;
}

void buildNetwork(int team) {
    int totalNodes = matchNum + teamNum + 2;
    //重新初始化
    capacity.assign(totalNodes, vector<int>(totalNodes, 0));
    edge.assign(totalNodes, vector<int>());
    int source = 0;//源点编号
    int sink = totalNodes - 1;//汇点编号
    int matchIndex = 1;//比赛节点起始编号
    int teamIndex = matchNum + 1;//队伍点起始编号
    //向网络中添加比赛节点
    for (auto match: matchNode) {
        //不考虑当前队伍比赛
        if (match.first.first != team && match.first.second != team) {
            //添加源点到比赛节点的边
            addEdge(source, matchIndex, match.second);
            //添加比赛节点到队伍节点的边
            addEdge(matchIndex, teamIndex + match.first.first, INF);
            addEdge(matchIndex, teamIndex + match.first.second, INF);
            matchIndex++;
        }
    }
    int maxPossibleWins = teamInfo[team].win + teamInfo[team].left;//当前队伍可能赢得的最多比赛
    //向网络中添加队伍节点
    for (int i = 0; i < teamNum; i++) {
        if (i != team) {
            //队伍节点到汇点的容量，即在确保team能赢的情况下该队伍还能赢的场数
            int capacityToSink = maxPossibleWins - teamInfo[i].win;
            if (capacityToSink < 0) capacityToSink = 0;
            //添加队伍节点到汇点的边
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
        //统计源点出发的流量
        for (auto match: matchNode) {
            if (match.first.first != i && match.first.second != i) {
                totalGames += match.second;
            }
        }
        //通过最大流获得到达汇点的最大流量
        int max_flow = maxFlow(source, sink);
        //源点出发的流量==到达汇点的流量，则不会被淘汰
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
