#include <iostream>
#include <vector>
#include <algorithm>

struct showTime {
    int start;
    int end;
};

int main() {
    int n;
    std::cin >> n;
    if (n == 0) {
        std::cout << 0 << std::endl;
        return 0;
    }
    if (n == 1) {
        std::cout << 1 << std::endl;
        return 0;
    }

    std::vector<showTime> arr(n);
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i].start >> arr[i].end;
    }

    std::sort(arr.begin(), arr.end(), [](showTime x1, showTime x2) {
        return x1.end < x2.end;
    });

    int count = 1;
    showTime lastTime = {arr[0].start, arr[0].end};
    for (int i = 1; i < n; i++) {
        showTime currentTime = arr[i];
        if (currentTime.start >= lastTime.end) {
            count++;
            lastTime = currentTime;
        }
    }

    std::cout << count << std::endl;
    return 0;
}