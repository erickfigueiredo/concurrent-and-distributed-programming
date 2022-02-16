#include <iostream>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

int amountFood, portion = 0;

sem_t foodOver;
sem_t hasFood;
sem_t somebodyEating;

void cook(int amount) {
    sem_wait(&foodOver);
    printf("Cozinheiro acordou!\n");

    if (amount) {
        printf("O cozinheiro reabasteceu %d.\n", amount);
        portion = amount;
        amountFood -= amount;
        sem_post(&hasFood);
    }

    printf("O cozinheiro dormiu!\n");
}

void cooker() {
    while (amountFood) {
        cook(min(5, amountFood));
    }
}

void eat(int id) {
    sem_wait(&somebodyEating);

    if (amountFood || portion) {

        if (!portion) {
            // Notifica o cozinheiro que a comida acabou
            sem_post(&foodOver);

            // Espera reestabelecer
            sem_wait(&hasFood);
        }

        portion--;
        printf("Canibal nº %d comeu!\n Restam %d na porção e %d no total\n", id, portion, amountFood);

    }

    sem_post(&somebodyEating);
}

void cannibal(int id) {
    while (amountFood || portion) {
        sleep(1);
        eat(id);
    }
}

int main() {
    sem_init(&foodOver, 0, 0);
    sem_init(&hasFood, 0, 0);
    sem_init(&somebodyEating, 0, 1);
    int totalCan;
    cout << "Informe a quantidade de comida disponível: ";
    cin >> amountFood;

    cout << "Informe a quantidade de canibais: ";
    cin >> totalCan;

    thread ckr(cooker);
    vector <thread> can;

    for (int i = 0; i < totalCan; i++)
        can.push_back(thread(cannibal, i + 1));

    ckr.join();

    for (int i = 0; i < totalCan; i++)
        can[i].join();

    return 0;
}