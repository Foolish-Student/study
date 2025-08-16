#include <iostream>
#include <vector>
#include <algorithm>

void qSort(std::vector<int> &a, int left, int right) {
    if (left >= right) {
        return;
    }

    int temp = a[left];
    int l = left, r = right;
    while (l < r) {
        while (l < r && a[r] > temp) {
            r--;
        }
        if (l < r) {
            a[l] = a[r];
            l++;
        }
        while (l < r && a[l] < temp) {
            l++;
        }
        if (l < r) {
            a[r] = a[l];
            r--;
        }
    }
    a[l] = temp;
    qSort(a, left, l - 1);
    qSort(a, l + 1, right);
}

int main() {
    int n;
    std::cin >> n;
    std::vector<int> a;
    a.reserve(n);
    for (int i = 0; i < n; i++) {
        int temp;
        std::cin >> temp;
        a.emplace_back(temp);
    }
    std::random_shuffle(a.begin(), a.end());
    qSort(a, 0, n - 1);
    for (int i: a) {
        std::cout << i << " ";
    }
    return 0;
}
