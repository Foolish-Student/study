#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;


struct pair_hash {
    size_t operator()(const std::pair<int,int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class MovieRentingSystem {
public:
    MovieRentingSystem(int n, vector<vector<int>>& entries) {
        for (int i = 0; i < entries.size(); i++) {
            vector<int> entrie = entries[i];
            m_price.insert({make_pair(entrie[0], entrie[1]), entrie[2]});
            m_visited[entrie[1]].insert(make_pair(entrie[2], entrie[0]));
        }
    }

    vector<int> search(int movie) {
        vector<int> result;
        result.reserve(5);
        auto p = m_visited[movie].begin();
        for (int i = 0; i < 5 && p != m_visited[movie].end(); i++, p++) {
            result.push_back(p->second);
        }

        return result;
    }

    void rent(int shop, int movie) {
        int price = m_price[make_pair(shop, movie)];
        m_visited[movie].erase({price, shop});
        m_unvisited.insert({price, shop, movie});
    }

    void drop(int shop, int movie) {
        int price = m_price[make_pair(shop, movie)];
        m_unvisited.erase({price, shop, movie});
        m_visited[movie].insert({price, shop});
    }

    vector<vector<int>> report() {
        std::cout << m_unvisited.size() << std::endl;

        vector<vector<int>> result;
        result.reserve(10);
        auto p = m_unvisited.begin();
        for (int i = 0; i < 5 && i < m_unvisited.size() && p != m_unvisited.end(); i++, p++) {
            result.push_back({get<1>(*p), get<2>(*p)});
        }
        return result;
    }

private:
    unordered_map<pair<int, int>, int, pair_hash> m_price;        // (shop, movie) -> price
    unordered_map<int, set<pair<int, int>>> m_visited; // price -> (shop, movie)
    set<tuple<int, int, int>> m_unvisited;             // (price, shop, movie)
};

/**
 * Your MovieRentingSystem object will be instantiated and called as such:
 * MovieRentingSystem* obj = new MovieRentingSystem(n, entries);
 * vector<int> param_1 = obj->search(movie);
 * obj->rent(shop,movie);
 * obj->drop(shop,movie);
 * vector<vector<int>> param_4 = obj->report();
 */