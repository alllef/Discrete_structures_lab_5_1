#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<Windows.h>
#include<limits>

using namespace std;

struct Rib {
public:
    int start;
    int end;
    double weight;
};

void initializeGraph(int &picks, int &ribs, vector<Rib> &structRibs);

void sortRibs(int &picks, int &ribs, vector<Rib> &structRibs);

void DijkstraAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs, int startPick);

int PickExists(int pick, vector<Rib> structRibs, vector<bool> isMarked, vector<bool> isRibUsed);

int findMin(const double *calculationsMatrix, int i, int picks);

void printMatrix(const double *matrix, int picks);

int main() {
    vector<Rib> ribsList;
    int n = 0, m = 0;
    initializeGraph(n, m, ribsList);
    sortRibs(n, m, ribsList);
    DijkstraAlgorithm(n, m, ribsList, 1);
    return 0;
}

void initializeGraph(int &picks, int &ribs, vector<Rib> &structRibs) {
    Rib myRib{};
    ifstream inFile;
    inFile.open("GraphWithScales.txt");

    if (!inFile.is_open()) cout << "It is not open" << endl;
    inFile >> picks >> ribs;

    for (int i = 0; i < ribs; i++) {
        inFile >> myRib.start >> myRib.end >> myRib.weight;

        structRibs.push_back(myRib);
    }
    inFile.close();
}

void sortRibs(int &picks, int &ribs, vector<Rib> &structRibs) {

    Rib tmp{};
    for (int i = 0; i < ribs - 1; i++) {
        for (int j = 0; j < ribs - 1; j++) {
            if ((structRibs[j].start + structRibs[j].end) > (structRibs[j + 1].start + structRibs[j + 1].end)) {

                tmp = structRibs[j];
                structRibs[j] = structRibs[j + 1];
                structRibs[j + 1] = tmp;

            }
        }
    }
}

void DijkstraAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs, int startPick) {

    double currentDistance = 0;
    int currentPick = startPick;
    vector<bool> isMarked(picks);
    vector<double> picksDistance(picks);
    auto *calculationsMatrix = new double[picks * picks];
    vector<bool> isRibUsed(structRibs.size());

    for (int i = 0; i < picks; i++) {
        for (int j = 0; j < picks; j++) {
            *(calculationsMatrix + i * picks + j) = DBL_MAX;
        }
    }

    picksDistance[currentPick - 1] = currentDistance;
    isMarked[currentPick - 1] = true;
    *(calculationsMatrix + 0 * picks + 0) = currentDistance;

    for (int i = 1; i < picks; i++) {
        while (PickExists(currentPick, structRibs, isMarked, isRibUsed)) {

            int ribIndex = PickExists(currentPick, structRibs, isMarked, isRibUsed) - 1;
            isRibUsed[ribIndex] = true;
            int pickIndex = structRibs[ribIndex].end - 1;

            if (*(calculationsMatrix + (i - 1) * picks + pickIndex) > currentDistance + structRibs[ribIndex].weight) {
                *(calculationsMatrix + i * picks + pickIndex) = currentDistance + structRibs[ribIndex].weight;
            } else *(calculationsMatrix + i * picks + pickIndex) = *(calculationsMatrix + (i - 1) * picks + pickIndex);
        }
        int minPickIndex = findMin(calculationsMatrix, i, picks);
        currentPick = minPickIndex + 1;
        currentDistance = *(calculationsMatrix + i * picks + minPickIndex);
        picksDistance[minPickIndex] = currentDistance;
        isMarked[minPickIndex] = true;
    }
    cout<<findMin(calculationsMatrix, 2, picks)<<endl;
    printMatrix(calculationsMatrix, picks);
}

int PickExists(int pick, vector<Rib> structRibs, vector<bool> isMarked, vector<bool> isRibUsed) {

    for (int i = 0; i < structRibs.size(); i++) {
        if (structRibs[i].start == pick && isMarked[structRibs[i].end - 1] == false && isRibUsed[i] == false) {

            return i + 1;
        }
    }
    return 0;
}

int findMin(const double *calculationsMatrix, int i, int picks) {
    int index = 0;
    for (int j = 0; j < picks - 1; j++) {
        if(i==2) cout<<*(calculationsMatrix +i * picks + j)<< " ";
        if ((*(calculationsMatrix +i * picks + j)) > (*(calculationsMatrix + i * picks + (j+1))))
            index = j+1;

    }
    cout<<endl;
    return index;
}

void printMatrix(const double *matrix, int picks) {
    for (int i = 0; i < picks; i++) {
        for (int j = 0; j < picks; j++) {
            cout << *(matrix + i * picks + j) << " ";
        }
        cout << endl;
    }
}