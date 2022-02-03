#include <mutex>
#include <ctime>
#include <thread>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <condition_variable>

using namespace std;

class PartyMonitor {
public:
    PartyMonitor() {
        cout << "PartyMonitor" << endl;
    }
    ~PartyMonitor() {
        cout << "~PartyMonitor" << endl;
    }

    void request(int d) {
        mux.lock();
        drinks[d] --;
        // código
        mux.unlock();
    }

    void free() {
        mux.lock();
        // código
        mux.unlock();
    }

    bool hasDrink() {
        return drinks[0]+drinks[1]+drinks[2] != 0;
    }

private:
    int drinks[3] = { 30, 30, 30 };
    // Manipula operações de Wait e Signal
    condition_variable isEmpty, isFull;
    mutex mux;
};

PartyMonitor pm;
bool isServing = true;

void garcom() {
    while(pm.hasDrink()) {
        int typeDrink = rand()%3+1;

        pm.request(typeDrink);
        // Copo precisa ter o tipo da bebida, se está cheio ou vazio
    }
}

void convidado() {
    while(pm.hasDrink()) {
        // N sei
        pm.free();
    }
}

int main() {
    srand(time(NULL));

    thread t(garcom);
    vector<pair<int, thread>>convidados;

    for(int i = 0; i < 3; i++) {
        convidados.push_back(make_pair(i, thread(convidado)));
    }

    t.join();

    for(auto &c : convidados) {
        c.second.join();
    }

    return 0;
}
