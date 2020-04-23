#include <iostream>
#include <vector>
#include <stack>
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

bool unMarked(vector<bool> &isMarked);

void DijkstraAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs, int startPick);

int PickExists(int pick, vector<Rib> &structRibs, vector<bool> &isMarked);

int findMin(vector<double> tmpVector);

int main() {
    vector<Rib> ribsList;
    int n = 0, m = 0;
    initializeGraph(n,m,ribsList);
    sortRibs(n,m,ribsList);
    DijkstraAlgorithm(n,m,ribsList,1);
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
    vector<double> tmpVector;



    for (int i = 0; i < picks; i++) {
        picksDistance[currentPick-1]=currentDistance;
        isMarked[currentPick - 1] = true;
        for (int j = 0; j < picks; j++) {

            if (PickExists(currentPick, structRibs, isMarked)) {
                int ribIndex = PickExists(currentPick, structRibs, isMarked);
                if (i > 0 &&
                    *(calculationsMatrix + (i - 1) * picks + j) > currentDistance + structRibs[ribIndex].weight) {
                    *(calculationsMatrix + i * picks + j) = currentDistance + structRibs[ribIndex].weight;
                    tmpVector.push_back(structRibs[ribIndex].weight + currentDistance);
                } else {
                    *(calculationsMatrix + i * picks + j) = *(calculationsMatrix + (i - 1) * picks + j);
                    tmpVector.push_back(*(calculationsMatrix + (i - 1) * picks + j));
                }
            } else {
                *(calculationsMatrix + i * picks + j) = DBL_MAX;
                tmpVector.push_back(DBL_MAX);
            }

        }
        currentPick = findMin(tmpVector) + 1;
        currentDistance = tmpVector[findMin(tmpVector)];
        tmpVector.clear();
    }
}

bool unMarked(vector<bool> &isMarked) {
    for (auto &&i : isMarked) {
        if (i == false) return true;
    }
    return false;
}

int PickExists(int pick, vector<Rib> &structRibs, vector<bool> &isMarked) {
    for (int i = 0; i < structRibs.size(); i++) {
        if (structRibs[i].start == pick && isMarked[structRibs[i].end - 1] == false) return i;

    }
    return 0;
}

int findMin(vector<double> tmpVector) {
    int index = 0;
    for (int i = 0; i < tmpVector.size() - 1; i++) {
        if (tmpVector[i] < tmpVector[index]) index = i;
    }
    return index;
}