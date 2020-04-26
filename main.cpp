#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<Windows.h>
#include<limits>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Rib {
public:
    int start;
    int end;
    double weight;
};

struct FloydAlgorithmResults {
public:
    double *distanceMatrix;
    int *historyMatrix;
};

struct DijkstraAlgorithmResults {
public:
    vector<int> path;
    vector<double> distance;
};

void initializeGraph(int &picks, int &ribs, vector<Rib> &structRibs);

void sortRibs(int &picks, int &ribs, vector<Rib> &structRibs);

DijkstraAlgorithmResults DijkstraAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs, int startPick);

int PickExists(int pick, vector<Rib> structRibs, vector<bool> isMarked, vector<bool> isRibUsed);

int findMin(const double *calculationsMatrix, int i, int picks);

double *makeContiguityWeightMatrix(int &picks, int &ribs, vector<Rib> &structRibs);

FloydAlgorithmResults FloydAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs);

int *setupHistoryMatrix(int picks);

void printShortestPathFloydAlgorithm(FloydAlgorithmResults results, int startPick, int endPick, int picks);

void printFloydMatrix(FloydAlgorithmResults results, int picks);

void printDijkstraAlgorithmPath(vector<int> path, int startPick, int endPick);

void printDijkstraShortestDistance(const vector<double> &distance, int startPick);

void makeMenu(int &picks, int &ribs, vector<Rib> &structRibs);

int main() {
    vector<Rib> ribsList;
    int n = 0, m = 0;
    SetConsoleOutputCP(CP_UTF8);
    makeMenu(n,m,ribsList);



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

DijkstraAlgorithmResults DijkstraAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs, int startPick) {

    double currentDistance = 0;
    int currentPick = startPick;
    vector<bool> isMarked(picks);
    vector<double> picksDistance(picks);
    auto *calculationsMatrix = new double[picks * picks];
    vector<bool> isRibUsed(structRibs.size());
    vector<int> path;

    for (int i = 0; i < picks; i++) {
        for (int j = 0; j < picks; j++) {
            *(calculationsMatrix + i * picks + j) = DBL_MAX;
        }
    }

    picksDistance[currentPick - 1] = currentDistance;
    isMarked[currentPick - 1] = true;
    *(calculationsMatrix + 0 * picks + 0) = currentDistance;
    path.push_back(currentPick);

    for (int i = 1; i < picks; i++) {
        while (PickExists(currentPick, structRibs, isMarked, isRibUsed)) {

            int ribIndex = PickExists(currentPick, structRibs, isMarked, isRibUsed) - 1;
            if (structRibs[ribIndex].weight < 0) return {};
            isRibUsed[ribIndex] = true;
            int pickIndex = structRibs[ribIndex].end - 1;

            if (*(calculationsMatrix + (i - 1) * picks + pickIndex) > currentDistance + structRibs[ribIndex].weight) {
                *(calculationsMatrix + i * picks + pickIndex) = currentDistance + structRibs[ribIndex].weight;
            } else *(calculationsMatrix + i * picks + pickIndex) = *(calculationsMatrix + (i - 1) * picks + pickIndex);
        }

        int minPickIndex = findMin(calculationsMatrix, i, picks);
        currentPick = minPickIndex + 1;
        path.push_back(currentPick);
        currentDistance = *(calculationsMatrix + i * picks + minPickIndex);
        picksDistance[minPickIndex] = currentDistance;
        isMarked[minPickIndex] = true;
    }
    DijkstraAlgorithmResults results{};
    results.distance = picksDistance;
    results.path = path;
    return results;
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
    vector<double> tmpVector(picks);

    for (int j = 0; j < picks; j++) {
        tmpVector[j] = *(calculationsMatrix + i * picks + j);
    }

    auto result = std::min_element(tmpVector.begin(), tmpVector.end());
    return std::distance(tmpVector.begin(), result);
}

double *makeContiguityWeightMatrix(int &picks, int &ribs, vector<Rib> &structRibs) {
    auto *contiguityWeightMatrix = new double[picks * picks];

    for (int i = 0; i < picks; i++) {
        for (int j = 0; j < picks; j++) {
            *(contiguityWeightMatrix + i * picks + j) = DBL_MAX;
            if (i == j)*(contiguityWeightMatrix + i * picks + j) = 0;
        }
    }
    for (int i = 0; i < ribs; i++) {
        if (structRibs[i].start != structRibs[i].end)
            *(contiguityWeightMatrix + (structRibs[i].start - 1) * picks +
              (structRibs[i].end - 1)) = structRibs[i].weight;
    }
    return contiguityWeightMatrix;
}

FloydAlgorithmResults FloydAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs) {

    auto *distanceMatrix = makeContiguityWeightMatrix(picks, ribs, structRibs);
    int *historyMatrix = setupHistoryMatrix(picks);

    for (int k = 0; k < picks; k++) {
        for (int i = 0; i < picks; i++) {
            for (int j = 0; j < picks; j++) {
                if (min(*(distanceMatrix + i * picks + j),
                        *(distanceMatrix + i * picks + k) +
                        *(distanceMatrix + k * picks + j)) != *(distanceMatrix + i * picks + j))
                    *(historyMatrix + i * picks + j) = k + 1;

                *(distanceMatrix + i * picks + j) = min(*(distanceMatrix + i * picks + j),
                                                        *(distanceMatrix + i * picks + k) +
                                                        *(distanceMatrix + k * picks + j));
                if (i == j && *(distanceMatrix) + i * picks + j < 0) {
                    *(distanceMatrix + i * picks + j) = 0;
                    break;
                }
            }
        }

    }

    FloydAlgorithmResults results{};
    results.distanceMatrix = distanceMatrix;
    results.historyMatrix = historyMatrix;
    return results;
}

int *setupHistoryMatrix(int picks) {
    int *setupMatrix = new int[picks * picks];
    for (int i = 0; i < picks; i++) {
        for (int j = 0; j < picks; j++) {
            *(setupMatrix + i * picks + j) = i + 1;
            if (i == j)*(setupMatrix + i * picks + j) = 0;
        }
    }
    return setupMatrix;
}

void printFloydMatrix(FloydAlgorithmResults results, int picks) {
    for (int k = 0; k < 2; k++) {
        if (k == 0) cout << " Матриця відстаней\n";
        else cout << "Матриця історії\n";
        for (int i = 0; i < picks; i++) {
            for (int j = 0; j < picks; j++) {
                if (k == 0) cout << setw(3) << *(results.distanceMatrix + i * picks + j);
                else cout << setw(3) << *(results.historyMatrix + i * picks + j);
            }
            cout << endl;
        }
        cout << endl;
    }
}

void printShortestPathFloydAlgorithm(FloydAlgorithmResults results, int startPick, int endPick, int picks) {
    vector<int> path;
    path.push_back(endPick);

    while (*(results.historyMatrix + (startPick - 1) * picks + (endPick - 1)) != startPick) {
        endPick = *(results.historyMatrix + (startPick - 1) * picks + (endPick - 1));
        path.push_back(endPick);
    }
    path.push_back(startPick);

    cout << "Маршрут від вершини " << startPick << " до " << path[0] << ": \n";
    for (int i = path.size() - 1; i >= 0; i--) {
        cout << path[i] << " ";
    }
    cout << endl;
}

void printDijkstraAlgorithmPath(vector<int> path, int startPick, int endPick) {
    cout << " Маршрут від вершини " << startPick << " до " << endPick << ": ";
    int i = 0;
    do {
        cout << path[i] << setw(2);
        i++;
    } while (path[i] != endPick);
    cout << path[i];
    cout << endl;

}

void printDijkstraShortestDistance(const vector<double> &distance, int startPick) {
    cout << "Відстань від " << startPick << " складає\n";
    for (int i = 1; i < distance.size(); i++) {
        cout << "До вершини " << i + 1 << ": " << distance[i] << endl;
    }
}

void makeMenu(int &picks, int &ribs, vector<Rib> &structRibs) {

    int startPick;
    int endPick;
    bool isFloyd;

    initializeGraph(picks, ribs, structRibs);
    sortRibs(picks, ribs, structRibs);

    cout << " Введіть 0 якщо хочете скористуватись алгоритмом Дійкстри та 1 якщо алгоритмом Флойда-Уоршелла";
    cin >> isFloyd;
    cout << endl;

    if (isFloyd) {
        FloydAlgorithmResults results = FloydAlgorithm(picks, ribs, structRibs);
        printFloydMatrix(results, picks);
        cout << "Введіть маршрут між вершинами який ви хочете побачити. Вершини можуть бути від 1 до " << picks;

        cin>>startPick>>endPick;
        printShortestPathFloydAlgorithm(results, startPick, endPick, picks);

    }
    else{
        cout<< "Введіть вершину від 1 до "<<picks<< "від якої хочете почати виконувати алгоритм";
        cin>>startPick;
        cout<<endl;
        DijkstraAlgorithmResults results = DijkstraAlgorithm(picks, ribs, structRibs, startPick);
        printDijkstraShortestDistance(results.distance, startPick);
        cout<< "Введіть вершину до якої хочете побачити маршрут з вершини"<<startPick;
        cin>>endPick;
        printDijkstraAlgorithmPath(results.path, startPick, endPick);
    }

}