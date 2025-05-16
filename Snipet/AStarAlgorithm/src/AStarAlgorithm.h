// A* (A-Star) Pathfinding Algorithm Example in C++ with Obstacles
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <stack>

struct Vec2 {
    int x, y;
    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }
};

namespace std {
    template<>
    struct hash<Vec2> {
        std::size_t operator()(const Vec2& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}

struct Node {
    Vec2 pos;
    float gCost; // from start
    float hCost; // to goal
    float fCost() const { return gCost + hCost; }
    Vec2 parent;

    bool operator>(const Node& other) const {
        return fCost() > other.fCost();
    }
};

std::vector<Vec2> GetNeighbors(const Vec2& pos, int width, int height) {
    std::vector<Vec2> result;
    const int dx[] = { 1, -1, 0, 0 };
    const int dy[] = { 0, 0, 1, -1 };

    for (int i = 0; i < 4; ++i) {
        int nx = pos.x + dx[i];
        int ny = pos.y + dy[i];
        if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
            result.push_back({ nx, ny });
        }
    }
    return result;
}

float Heuristic(const Vec2& a, const Vec2& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y); // Manhattan distance
}

std::vector<Vec2> ReconstructPath(const std::unordered_map<Vec2, Vec2>& cameFrom, Vec2 current) {
    std::vector<Vec2> path;
    while (cameFrom.count(current)) {
        path.push_back(current);
        current = cameFrom.at(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Vec2> AStar(const std::vector<std::vector<int>>& grid, Vec2 start, Vec2 goal) {
    int width = static_cast<int>(grid[0].size());
    int height = static_cast<int>(grid.size());

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Vec2, float> gScore;
    std::unordered_map<Vec2, Vec2> cameFrom;
    std::unordered_set<Vec2> closed;

    gScore[start] = 0.0f;
    openSet.push({ start, 0.0f, Heuristic(start, goal), {-1, -1} });

    while (!openSet.empty()) {
        Node current = openSet.top(); 
        openSet.pop();
        if (current.pos == goal) {
            return ReconstructPath(cameFrom, goal);
        }

        if (closed.count(current.pos)) continue;
        closed.insert(current.pos);

        for (const Vec2& neighbor : GetNeighbors(current.pos, width, height)) {
            if (grid[neighbor.y][neighbor.x] == 1) continue; // wall/obstacle
            if (closed.count(neighbor)) continue;

            float tentative_g = gScore[current.pos] + 1.0f;

            if (!gScore.count(neighbor) || tentative_g < gScore[neighbor]) {
                gScore[neighbor] = tentative_g;
                float h = Heuristic(neighbor, goal);
                openSet.push({ neighbor, tentative_g, h, current.pos });
                cameFrom[neighbor] = current.pos;
            }
        }
    }

    return {}; // no path
}

//-----------------------------------------------
// 예제 main: 장애물 포함
//-----------------------------------------------
int main() {
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 1},
        {0, 0, 0, 0, 0},
    };

    Vec2 start = {0, 0};
    Vec2 goal = {4, 4};

    auto path = AStar(grid, start, goal);
    if (path.empty()) {
        std::cout << "No path found\n";
    } else {
        std::cout << "Path: ";
        for (auto& p : path) {
            std::cout << "(" << p.x << "," << p.y << ") ";
        }
        std::cout << "\n";
    }

    return 0;
}
