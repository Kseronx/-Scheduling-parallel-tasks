#include <iostream>
#include <vector> 
#include <fstream> 
#include <string>
#include <cstdlib> 
#include <signal.h> 
#include <stdlib.h> 
#include <chrono>
#include <algorithm>
#include <climits>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>
#include <numeric>
#include <sstream>
#include <random>
#include <unistd.h>


using namespace std;

vector<vector<int>>Tasks;
vector<vector<int>>usedproc;
vector<bool>Status;
vector<vector<int>>ReadyTasks;
vector<vector<int>>proc;

bool quit = false;
int MaxProcs = 0;
int cost = 0;
int MaxJobs = 0;

struct Task {
    int id;
    int duration;
    int ready_time;
    int processors_requested;
};

// Struktura opisująca rozwiązanie (uszeregowanie zadań)
struct Solution {
    std::vector<int> task_order;                        // Kolejność zadań
    std::vector<int> task_starts;                       // Moment rozpoczęcia każdego zadania
    std::vector<int> task_ends;                         // Moment zakończenia każdego zadania
    std::vector<std::vector<int>> processors_assigned;  // Przydzielone procesory dla każdego zadania
    int makespan;                                       // Czas trwania całego uszeregowania
    int cost;                                           // Koszt rozwiązania (dla celów optymalizacji)



    // Konstruktor tworzący początkowe rozwiązanie (kolejność według momentu gotowości)
    Solution(const std::vector<Task>& tasks, int max_processors) : task_order(tasks.size()), task_starts(tasks.size()), task_ends(tasks.size()), processors_assigned(tasks.size()), makespan(0), cost(0) {
          vector<vector<int>>Tasks1;
        vector<int>temporary;
        for (int i = 0; i < Tasks.size(); i++) {
            temporary.push_back(Tasks[i][0]);
            temporary.push_back(Tasks[i][1]);
            temporary.push_back(Tasks[i][2]);
            temporary.push_back(Tasks[i][3]);
            Tasks1.push_back(temporary);
            temporary.clear();
        }
        //int max_processors = MaxProcs;
        //algorytm zachlanny
        for (int i = 0; i < Tasks1.size(); i++) {
            Status.push_back(false);
        }
        cost = 0;
        int Procs = MaxProcs;
        vector<int> procesory;
        vector<int>temp2;
        vector<int>temp3;
        for (int i = 0; i < Procs; i++) {
            procesory.push_back(0);
        }

        for (int z = 0; Tasks1.size() > z;) // jesli sa elementy w liscie z zadaniami
        {
            for (int i = 0; i < Tasks1.size(); i++) // przechodzimy przez wszystkie zadania
            {
                //sprawdzamy czy zadanie jest zakonczone a pozniej zwalniamy procesory
                if (Tasks1[i][2] == 0) { //jesli czas trwania zadania jest rowny 0
                    Procs += Tasks1[i][3]; //do zmiennej procs zwracamy procesory ktore zakonczyly prace
                    for (int j = 0; j < Tasks1[i][3]; j++) {
                        procesory[usedproc[i][j]] = 0; // zwracamy procesory do ponownego uzycia
                    }
                    Tasks1.erase(Tasks1.begin() + i); //odejmujemy zadanie, status, usedproc z poczatku vectora 
                    Status.erase(Status.begin() + i);
                    usedproc.erase(usedproc.begin() + i);
                    i--;
                };
            };

            for (int i = 0; i < Tasks1.size(); i++) { //przechodzimy przez wszystkie zadania
                if (Procs == 0) { //sprawdzanie czy wszystkie procesory sa zajete
                    break;
                }
                //jesli wystarczy procesorow sprawdzamy czy zadanie jest gotowe i jaki ma status
                // oraz dodajemy do readytaskow jesli wszystko sie zgadza
                if (Procs >= Tasks1[i][3] && Status[i] == false && Tasks1[i][1] <= cost) {
                    int temp = 0;
                    for (int j = 0; j < MaxProcs; j++) {
                        if (procesory[j] == 0 && temp < Tasks1[i][3]) {
                            procesory[j] = Tasks1[i][2]; //do kazdego procesora dodajamy run time zadan 
                            usedproc[i][temp] = j; //tutaj dodajemy do usedproc procesory ktore uzylismy
                            temp++;
                        }
                        else if (temp >= Tasks1[i][3])
                            break;
                    };


                    temp2.push_back(Tasks1[i][0]); //dodajemy nr zadania
                    temp2.push_back(cost); // dodajemy czas rozpoczecia zadania
                    temp2.push_back(cost + Tasks1[i][2]); //dodajemy czas zakonczenia zadania

                    for (int j = 0; j < usedproc[i].size(); j++) { //dodajemy do readytaskow liste uzytych procesorow
                        temp3.push_back(usedproc[i][j]);
                    }
                    proc.push_back(temp3);
                    temp3.clear();
                    ReadyTasks.push_back(temp2);
                    temp2.clear();
                    Status[i] = true; //zmieniamy status zadania na true
                    Procs -= Tasks1[i][3]; //odemujemy liczbe zajetych procesorow
                };
            };
            //tutaj szukamy procesorow ktore skoncza sie najszybciej
            int min = 0;
            // przyjmujemy pierwsza dodatnia wartosc dla min
            int i = 0;
            while (i < procesory.size()) {
                if (procesory[i] > 0) {
                    min = procesory[i];
                    break;
                };
                i++;
            }
            i = 0;
            while (i < procesory.size()) { //w procesorach mamy zapisane czasy run time zadan
                if (procesory[i] < min && procesory[i] > 0) { //szukamy najmniejszego run time w procesorach
                    min = procesory[i]; //znajdujemy najmniejszy run time procesora
                }
                i++;
            }
            i = 0;
            while (i < Tasks1.size()) {
                if ((Tasks1[i][1] > cost) && (Status[i] == false) && (Tasks1[i][1] - cost < min || min == 0)) {
                    min = Tasks1[i][1] - cost; //tutaj wybieramy zadanie ktore przybedzie kolejne 
                };
                i++;
            };
            // odejmujemy min od czasu run taskow
            i = 0;
            while (i < Tasks1.size()) {
                if (Status[i] == true) {
                    Tasks1[i][2] -= min; //tu odejmujemy czas zwolnienia procesoru od run time kazdego zadania jest ma status true czyli jest wykonywane
                }
                i++;
            }

            //tu odejmujemy ten czas min od kazdego procesora
            i = 0;
            while (i < procesory.size()) {
                if (procesory[i] > 0) {
                    procesory[i] -= min;
                }
                i++;
            }
            cost += min;
            if (min == 0) {
                break;
            }
            makespan = cost;
        };
        cout << "czas zakonczenia: " << cost << "\n";
        for (int i = 0; i < ReadyTasks.size(); i++) {
            task_order[i] = ReadyTasks[i][0];
            task_starts[i] = ReadyTasks[i][1];
            task_ends[i] = ReadyTasks[i][2];
            for (int j = 0; j < proc[i].size(); j++) {
                processors_assigned[i].push_back(proc[i][j]);
            }
        }
    }
   
};
       

void ustawienia() { //funkcja pomocnicza tworzaca vector usedproc
    vector<int>temp;
    for (int i = 0; i < Tasks.size(); i++) {
        for (int j = 0; j < Tasks[i][3]; j++) {
            temp.push_back(-1);
        }
        usedproc.push_back(temp);
        temp.clear();
    }
}

//using namespace std;


bool operator==(const Solution& lhs, const Solution& rhs) {
    return lhs.task_order == rhs.task_order && lhs.task_starts == rhs.task_starts && lhs.task_ends == rhs.task_ends && lhs.processors_assigned == rhs.processors_assigned && lhs.makespan == rhs.makespan && lhs.cost == rhs.cost;
}

// Funkcja obliczająca koszt rozwiązania (dla celów optymalizacji)
int Cost(const std::vector<Task>& tasks, const Solution& solution, vector<vector<int>>ReadyTasks) {
    int cost = 0;
    int time = 0;
    for (int i = 0; i < ReadyTasks.size(); i++) {  //
        //const auto& task = tasks[solution.task_order[i]-1]; //zamiast i było: "solution.task_order[i]", "tasks[i]"
        const auto& task = ReadyTasks[i]; //"solution.task_order[i]" to było zamiast "i" 
        time = std::max(time, ReadyTasks[i][1]); // zamiast readytask powinno być "task.ready_time"
        cost += time - ReadyTasks[i][1];
        time += ReadyTasks[i][2]- ReadyTasks[i][1];
    }
    return cost;
}

// Funkcja generująca sąsiednie rozwiązania poprzez zamianę dwóch zadań
std::vector<Solution> GenerateNeighbors(const std::vector<Task>& tasks, const Solution& solution, int tabu_list_size, int max_iterations, int max_sasiad) {
    std::vector<Solution> neighbors;
    for (int i = 0; i < max_sasiad ; i++) { //solution.task_order.size() //solution.task_order.size()
            int z = rand() % Tasks.size();
            //int y = rand() % tasks.size();
            Solution neighbor = solution;
            if(Tasks.size()>2 && z!=0){
            std::swap(neighbor.task_order[z], neighbor.task_order[z-1]);
            }
            neighbor.cost = Cost(tasks, neighbor,ReadyTasks);
            neighbors.emplace_back(neighbor);
        }
    
    return neighbors;
}

// Algorytm tabu search do znajdowania optymalnego uszeregowania zadań równoległych
Solution TabuSearch(const std::vector<Task>& tasks, int max_processors, int tabu_list_size, int max_iterations, int max_sasiad) {
    Solution best_solution(tasks, max_processors);
    std::vector<Solution> tabu_list;
    for (int iteration = 0; iteration < max_iterations; iteration++) {
        std::vector<Solution> neighbors = GenerateNeighbors(tasks, best_solution,  tabu_list_size,  max_iterations, max_sasiad);
        Solution best_neighbor = best_solution;
        for (const auto& neighbor : neighbors) {
            if (std::find(tabu_list.begin(), tabu_list.end(), neighbor) == tabu_list.end()) {
                if (neighbor.cost < best_neighbor.cost) {
                    best_neighbor = neighbor;
                }
            }
        }
        if (best_neighbor.cost < best_solution.cost) {
            best_solution = best_neighbor;
        }
        tabu_list.emplace_back(best_neighbor);
        //for (int i = 0; i < tabu_list.size(); i++) {
            if (tabu_list.size() > tabu_list_size) {
                tabu_list.erase(tabu_list.begin());
            //}
        }
    }
    return best_solution;
}

//vector<vector<int>>Tasks;



void LoadFromFile(string filename, int& N_task) {
    ifstream plik(filename);
    int temp1 = 0;
    vector<int>temp;

    if (!plik.good()) {
        std::cout << "Blad wczytywania.\n" << endl;
    }
    string linia;
    while (getline(plik, linia))
    {

        stringstream bufor(linia);
        if (bufor.str().size() < 3) continue;
        string job = "MaxJobs:";
        string numprocs = "MaxProcs:";

        size_t pozycja = linia.find(numprocs);
        if (pozycja == string::npos) // jesli nie znaleziono
        {
            pozycja = linia.find(job);
            if (pozycja == string::npos) {
                pozycja = linia.find(";");
                if (pozycja == string::npos) {
                    int indeks = 0;
                    string str = "";
                    for (int i = 0; i < linia.size(); i++) {
                        if ((linia[i] >= '0' && linia[i] <= '9') || int(linia[i]) == 45)
                            str += linia[i];
                        else if (str.size() > 0) {
                            if (indeks == 0) {
                                temp.push_back(atoi(str.c_str()));
                            }
                            else if (indeks == 1) {
                                temp.push_back(atoi(str.c_str()));
                            }
                            else if (indeks == 3) {
                                temp.push_back(atoi(str.c_str()));
                            }
                            else if (indeks == 4)
                                temp.push_back(atoi(str.c_str()));
                            indeks++;
                            str = "";
                        }
                    }
                    if ((temp[0] > 0 || temp[2] > 0 || temp[3] > 0) && temp[2] > 0 && temp[3] > 0) {
                        Tasks.push_back(temp);
                        temp1++;
                    };
                }
            }
            else {
                string liczba;
                for (int i = static_cast<int>(pozycja + job.size()); i < linia.size(); i++)
                    liczba += linia[i];
                MaxJobs = atoi(liczba.c_str());
                if (N_task == -1)
                    N_task = MaxJobs;
            }

        }
        else {
            string liczba;
            for (int i = static_cast<int>(pozycja + numprocs.size()); i < linia.size(); i++)
                liczba += linia[i];
            MaxProcs = atoi(liczba.c_str());
        }

        if (temp1 == N_task || plik.eof()) {
            N_task = temp1;
            break;
        }
        temp.clear();
    }
    perror("File open");
    plik.close();
}
void stop(int signo) {
    cout << "Sygnał stop\n";
    quit = true; // dzięki temu podaje wynik
}

int main(int argc, char* argv[]) {  // ./nazwa_programu N_task "fileName" 
    string filename = argv[2];
    int N_task = atoi(argv[1]);

//int main() { 

   // string filename = "NASA-iPSC-1993-2.1-cln.swf";
   // int N_task = -1;

    LoadFromFile(filename, N_task);
    ustawienia();
    while (N_task < 0 || N_task > MaxJobs) {
        N_task = MaxJobs;
    }

    cout << "N_task = " << N_task << endl;
    cout << "MaxJobs = " << MaxJobs << endl;
    cout << "MaxProcs = " << MaxProcs << endl;

    srand(time(NULL));

    // Wczytanie danych
    vector<Task> tasks;

     for (int i = 0; i < Tasks.size(); i++)
    {
        Task t;
        t.id = Tasks[i][0];
        t.duration = Tasks[i][2];
        t.ready_time = Tasks[i][1];
        t.processors_requested = Tasks[i][3];
        tasks.push_back(t);
    }

    // Znajdź optymalne uszeregowanie zadań przy użyciu algorytmu tabu search
    int max_sasiad = 30;
    int max_processors = MaxProcs;
    int tabu_list_size = 60;
    int max_iterations = 6;
    if (tabu_list_size > N_task) {
        tabu_list_size = N_task;
    }
    signal(SIGALRM, stop);
    alarm(300); // 5 min 

    auto s = chrono::high_resolution_clock::now();
    Solution solution = TabuSearch(tasks, max_processors, tabu_list_size, max_iterations, max_sasiad);
    auto e = chrono::high_resolution_clock::now();

    cout << "czas tabu search: " << chrono::duration_cast<chrono::milliseconds>(e - s).count() << endl;
   
   
    // Zapisz wynik do pliku (numer zadania, moment startu, moment końca, przydzielone procesory)
    std::ofstream out("schedule.txt");
    for (int i = 0; i < tasks.size(); i++) {
        out << ReadyTasks[i][0] << " " << solution.task_starts[i] << " " << solution.task_ends[i] << " "; //tasks[i].id , solution.task_order[i]
        for (const auto& processor : solution.processors_assigned[i]) {
            out << processor << " ";
        }
        out << std::endl;
    }

    return 0;
}
