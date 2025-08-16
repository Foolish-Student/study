#include <iostream>
#include <vector>
#include <algorithm>

void Merge(std::vector<int> &a, int left, int mid, int right) {
    std::vector<int> temp;
    int l = left, r = mid + 1;
    while (l <= mid && r <= right) {
        if (a[l] <= a[r]) {
            temp.push_back(a[l++]);
        }else {
            temp.push_back(a[r++]);
        }
    }

    while (l <= mid) {
        temp.push_back(a[l++]);
    }
    while (r <= right) {
        temp.push_back(a[r++]);
    }

    std::copy(temp.begin(), temp.end(), a.begin() + left);
}

void MergeSort(std::vector<int> &a, int left, int right) {
    if (left >= right) {
        return;
    }

    int mid = (left + right) / 2;
    MergeSort(a, left, mid);
    MergeSort(a, mid + 1, right);
    Merge(a, left, mid, right);
}

int main() {
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) {
        int temp;
        std::cin >> temp;
        a[i]=(temp);
    }
    MergeSort(a, 0, n - 1);
    for (int i: a) {
        std::cout << i << " ";
    }
    return 0;
}