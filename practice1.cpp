#include <thread>
#include <future>
#include <vector>
#include <iostream>

using namespace std;

void sumPartitionValue(const vector<int>& a, int id, int ths, promise<long long>&& p) {
    long long sum = 0;

    int begin = (a.size() / ths) * id;
    int end = (ths - 1 == id) ? a.size() : (a.size() / ths) * (id + 1);

    for (int i = begin; i < end; i++) sum += a[i];

    p.set_value(sum);
}

int main() {
    int n = 0, nThreads;
    long long sum = 0;

    cout << "Informe o número de elementos da lista e o número de threads: ";
    cin >> n >> nThreads;

    vector <int> arr;
    vector <thread> threads;
    vector <future<long long>> futures;
    vector <promise<long long>> promises;

    for (int i = 0; i < n; i++) {
        arr.push_back(i + 1);
    }

    chrono::time_point<chrono::high_resolution_clock> timeInit, timeEnd;
    timeInit = chrono::high_resolution_clock::now();

    for (int i = 0; i < nThreads; i++) {
        promises.push_back(promise<long long>());
        futures.push_back(promises[i].get_future());

        threads.push_back(thread(sumPartitionValue, ref(arr), i, nThreads, move(promises[i])));
    }

    for (future<long long>& ft : futures) {
        sum += ft.get();
    }

    timeEnd = chrono::high_resolution_clock::now();

    for (thread& th : threads) {
        th.join();
    }

    double timeTotal = chrono::duration<double, ratio <1, 1000>>(timeEnd-timeInit).count();

    cout << "> Tempo total de Execução: " << timeTotal << endl;
    cout << "A soma é " << sum << endl;
    cout << "Verificando por Soma de P.A, temos: " << ((long long)(arr[0] + arr[n - 1]) * n) / 2 << endl;
    return 0;
}