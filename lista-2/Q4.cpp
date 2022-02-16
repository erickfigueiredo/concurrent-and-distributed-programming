#include <mutex>
#include <vector>
#include <thread>
#include <iostream>
#include <condition_variable>

using namespace std;

class BarbershopMonitor {
public:
    BarbershopMonitor(int limit, int total) {
        limitClients = limit;
        totalClients = total;
    }

    void request(int id) {
        unique_lock<mutex>lck(m);

        printf("Cliente nº%d chegou!\n", id);


        // Se não puder esperar, vai embora
        if (!canWait()) { rej++; return; };

        printf("Cliente nº%d conseguiu um lugar na fila!\n", id);

        // Se puder esperar incrementa quem está esperando
        currWaiting++;
        // Tudo que atendendu tudo que foi emborament

        cv.wait(lck, [this] {
            return !isBusy;
            });

        isBusy = true;

        currWaiting--;

        printf("Chegou a vez de Cliente nº%d cortar o cabelo!\n", id);

        bv.notify_one();
    }

    void free() {
        unique_lock<mutex>lck(m);

        while (totalClients - (rej + totalCut)) {
            bv.wait(lck, [this] {
                if(!isBusy) {
                    isAwake = false;
                    cout << "O barbeiro voltou a dormir!\n";
                }
                return isBusy;
                });

            totalCut++;

            if (!isAwake) {
                isAwake = true;
                cout << "O Barbeiro acordou...\n";
            }

            cout << "Cortou o cabelo do cliente...\n";

            isBusy = false;

            cv.notify_all();
        }
    }


private:


    bool canWait() {
        return currWaiting < limitClients;
    }

    condition_variable cv, bv;
    int limitClients, currWaiting = 0, totalCut = 0, rej = 0, totalClients;
    bool isBusy = false, isAwake = false;
    mutex m;
};


int totalClients = 15;

BarbershopMonitor bs(10, totalClients);

void barber() {
    bs.free();
}

void client(int id) {
    bs.request(id);
}

int main() {
    vector <thread> clients;

    thread b(barber);

    for (int i = 0; i < totalClients; i++)
        clients.push_back(thread(client, i + 1));

    b.join();

    for (int i = 0; i < totalClients; i++)
        clients[i].join();

    return 0;
}