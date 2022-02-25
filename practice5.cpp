#include <set>
#include <cmath>
#include <omp.h>
#include <iostream>
#include <chrono>

using namespace std;

/**
 * n = 100
 * Serial = 0.0001752;
 * Parallel = 0.0145226;
 * 
 * n = 10000
 * Serial = 0.0027652;
 * Parallel = 0.0267402;
 * 
 * n = 1000000
 * Serial = 0.0199444;
 * Parallel = 0.359795;
 */


int main() {
    int n, root, count = 0;
    set <int> primes;

    cin >> n;

    auto start = chrono::system_clock::now();
#pragma omp parallel for default(none) schedule(guided, omp_get_num_threads()) private(count) shared(n, primes, cout)
    for (int i = 2; i < n; i++) {
        for (int j = 1; j <= (int)sqrt(i); j++) {
            if (i != 2 && i % 2 == 0) { count = 2; break; }
            if (i % j == 0) {
                count++;
                if (count > 1) break;
            }
        }

        if (count == 1) {
#pragma omp critical
            primes.insert(i);
        }

        count = 0;
    }

    auto end = chrono::system_clock::now();
    chrono::duration<double> time = end - start;

    cout << time.count() << '\n';

    for (auto i : primes) cout << i << ' ';
    cout << '\n';

    return 0;
}
