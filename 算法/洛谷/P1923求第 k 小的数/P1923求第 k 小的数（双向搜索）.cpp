#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 5e6 + 10;
int a[MAXN];
int n, k;

int quick_select(int l, int r) {
    if (l >= r)
        return a[l];
    int pivot = a[l];
    int i = l, j = r;
    while (i < j) {
        while (i < j && a[j] > pivot)
            j--;
        if (i < j) {
            swap(a[i], a[j]);
            i++;
        }
        while (i < j && a[i] < pivot)
            i++;
        if (i < j) {
            swap(a[i], a[j]);
            j--;
        }
    }
    a[i] = pivot; // 恢复枢轴

    // 根据k位置递归
    if (k == i || k == j)
        return a[k]; // 找到第k小的数

    if (k < j)
        return quick_select(l, j - 1);
    if (k > i)
        return quick_select(i + 1, r);
    return a[k]; // k在中间区域直接命中
}

int main() {
    scanf("%d %d", &n, &k);
    for (int i = 0; i < n; ++i)
        scanf("%d", &a[i]);

    printf("%d", quick_select(0, n - 1));

    return 0;
}

