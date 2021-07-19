#pragma once
#include "cell.hpp"
#include "Algo.hpp"

class DisjSet {
    std::vector<int> rank, parent;
    int n, imax, jmax;

public:
    DisjSet() {}
    DisjSet(int imax, int jmax) : imax(imax), jmax(jmax)
    {
        rank = std::vector<int>(imax * jmax);
        parent = std::vector<int>(imax * jmax);
        this->n = imax * jmax;
        for (int i = 0; i < imax; i++) {
            for (int j = 0; j < jmax; j++) {
                make_set(i * jmax + j);
            }
        }
    }

    void make_set(int v) {
        parent[v] = v;
        rank[v] = 1;
    }

    int find_set(int v) {
        while (v != parent[v]) {
            v = parent[v];
        }
        return v;
    }

    bool union_sets(int a, int b) {
        a = find_set(a);
        b = find_set(b);
        if (a != b) {
            if (rank[a] < rank[b]) {
                int temp = a;
                a = b;
                b = temp;
            }
            parent[b] = a;
            rank[a] += rank[b];
            return true;
        }
        else {
            return false;
        }
    }

    void setSameColor(std::vector<std::vector<cell>>& cells) {
        for (int i = 0; i < n; i++) {
            int j = find_set(i);
            sf::Vector2i parrent = convertIndex(j);
            sf::Vector2i child = convertIndex(i);
            if (rank[j] > 1) {
                if (cells[parrent.x][parrent.y].cCol.a == 0) {
                    cells[parrent.x][parrent.y].setColor(HSV_TO_RGB(getRandom() * 360.0f, 100.0f, 100.0f));
                }
                cells[child.x][child.y].setColor(cells[parrent.x][parrent.y].cCol);
            }
        }
    }

    sf::Vector2i convertIndex(int n) {
        int j = n % jmax;
        int i = (n - j) / jmax;
        return { i, j };
    }
};