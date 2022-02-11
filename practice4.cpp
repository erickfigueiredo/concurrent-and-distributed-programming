#include <thread>
#include <vector>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

int shared_variable = 0, count, N;

sem_t s_main;

void consumer(int id) {
    while (true) { // Mantém a thread que não puder seguir em espera
        sem_wait(&s_main);
        if (count - id >= 0) { // Se ela atender à condição
            printf("> Thread %d entrou <\n", id);
            count -= id; // Decrementa e sai do semáforo
            sem_post(&s_main);
            break;
        }
        else if (id > N) { // Se a thread ultrapassar o limit, ela sai do semáforo e da função
            sem_post(&s_main);
            return;
        }
        sem_post(&s_main);
    }

    sleep(1);

    sem_wait(&s_main);
    shared_variable += id; // opera sobre a variavel compartilhada
    printf("Thread %d passou\nVariavel compartilhada = %d - count = %d.\n\n", id, shared_variable, count);
    count += id; // devolve o valor ao count
    sem_post(&s_main);
}

int main() {
    sem_init(&s_main, 0, 1);

    vector<thread> threads;
    int amount_threads;
    cin >> amount_threads >> N;

    count = N;

    for (int i = 0; i < amount_threads; i++)
        threads.push_back(thread(consumer, i + 1)); // Aloca as threads com os ids

    for (auto& t : threads) t.join(); // Libera

    sem_destroy(&s_main); //Destroi o semáforo

    assert((N * (N + 1)) / 2 == shared_variable); // Confere o valor

    cout << shared_variable << endl;
    return 0;
}
