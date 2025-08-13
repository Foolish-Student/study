#include "P1923求第 k 小的数（双向搜索）.cpp"


int quick_select2(int l, int r) {
    int left = l;
    int pivot = a[r];

    // left的左边一定比pivot小，右边一定比pivot大或等
    for (int j = l; j < r; j++) {
        if (a[j] < pivot) {
            std::swap(a[j], a[left++]);
        }
    }

    std::swap(a[left], a[r]);
    if (k == left) {
        return a[left];
    } else if (k < left) {
        return quick_select2(l, left - 1);
    } else {
        return quick_select2(left + 1, r);
    }
}