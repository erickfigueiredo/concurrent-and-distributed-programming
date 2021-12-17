#include <iostream>
#include <queue>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include <ctime>

using namespace std;

pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t muxrec = PTHREAD_MUTEX_INITIALIZER;
map<pthread_t, int> A;
queue<pthread_t> fila[5]; // 1 Fila para cada recurso
bool rec[5] = { 1, 1, 1, 1, 1 };

void block() {
    bool sair = false;
    long eu = pthread_self();

    do {
        pthread_mutex_lock(&mux);
        if (A[eu] > 0) {
            A[eu]--;
            sair = true;
        }
        pthread_mutex_unlock(&mux);
    } while (!sair);
}

void wakeup(pthread_t t) {
    pthread_mutex_lock(&mux);
    A[t]++;
    pthread_mutex_unlock(&mux);
}

void libera(int R) {
    pthread_mutex_lock(&muxrec);

    rec[R] = true;  // Libera o recurso

    // Se existir uma pilha com elementos
    if (!fila[R].empty()) {
        pthread_t id = fila[R].front();
        fila[R].pop(); // Remove o primeiro da fila de espera

        wakeup(id); // Acorda o primeiro da fila
    }
    pthread_mutex_unlock(&muxrec);
}

int requisita(int R) {
    int r;
    bool bloquear;

    do {
        bloquear = false;
        pthread_mutex_lock(&muxrec);
        if (rec[R]) { // livre
            rec[R] = false;
            r = 1;
        }
        else { // ocupado
            bloquear = true;
            fila[R].push(pthread_self());
        }
        pthread_mutex_unlock(&muxrec);

        if (bloquear) block();
    } while (bloquear);
    return r;
}

void* func(void* id) {
    int v = rand() % 5;
    int u;
    u = requisita(v);
    cout << "[" << (long) id <<"] usando r = " << v << " u = " << u << endl;
    usleep(10);
    libera(v);

    return nullptr;
}

int main() {
    srand(time(NULL));
    // pthread_mutex_init(&muxrec, NULL);

    int num = 5;

    for (int i = 0;i < num;++i) {
        pthread_t t;
        pthread_create(&t, NULL, func, (void*)i);
        A.insert(pair<pthread_t, int>(t, 0));
    }

    for (auto& p : A) {
        pthread_join(p.first, NULL);
    }
    // pthread_mutex_destroy(&muxrec);

    return 0;
}
