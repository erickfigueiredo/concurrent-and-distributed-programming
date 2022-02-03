#include <mutex>
#include <ctime>
#include <thread>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <condition_variable>

using namespace std;

class DrinkMonitor {
public:
    DrinkMonitor() {
        cout << "Festa começou\n";
    }

    ~DrinkMonitor() {
        cout << "Fim de festa\n";
    }

    // Encher o Copo
    bool request(int drinkIndex) {
        unique_lock<mutex>lck(m);

        if (drinks[drinkIndex]) {
            // Aguarda o copo ficar vazio
            emptyGlass.wait(lck, [this]()->bool {
                return isGlassEmpty;
                });

            cout << drinks[0] << ' ' << drinks[1] << ' ' << drinks[2] << endl;
            cout << "Encheu o copo com " << drinkIndex << endl;

            drinks[drinkIndex]--;
            drinkType = drinkIndex;

            isGlassEmpty = false;

            fullGlass.notify_all();
            if (!hasDrink()) { return true; }
        }
        return false;
    }

    // Tomar a bebida
    void free(int id) {
        unique_lock<mutex>lck(m);

        fullGlass.wait(lck, [this, id]()->bool {
            return (id >= drinkType && !isGlassEmpty) || !hasDrink();
            });

        if (isPartyFinished) return;

        cout << id << " bebendo " << drinkType << endl;

        isGlassEmpty = true;
        emptyGlass.notify_one();
    }

    bool hasDrink() {
        return drinks[0] + drinks[1] + drinks[2] != 0;
    }

    void setPartyStatus(bool stts) {
        isPartyFinished = stts;
    }

    bool getPartyStatus() {
        return isPartyFinished;
    }

private:
    int drinks[3] = { 30, 35, 40 };
    int drinkType = -1;
    bool isGlassEmpty = true, isPartyFinished = false;
    condition_variable emptyGlass, fullGlass;
    mutex m;
};

DrinkMonitor dm;

void waiter() {
    while (true) {
        int typeDrink = rand() % 3;
        if (dm.request(typeDrink)) break;
    }

    dm.setPartyStatus(true);
}

void guest(int id) {
    while (!dm.getPartyStatus()) {
        dm.free(id);
    }
}

int main() {
    srand(time(NULL));

    thread wtr(waiter);
    vector<thread> guests;

    for (int i = 0; i < 3; i++) { guests.push_back(thread(guest, i)); }
    wtr.join();

    for (auto& g : guests) { g.join(); }

    return 0;
}