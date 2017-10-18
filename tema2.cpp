// Copyright 2017 Medar Andrei
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// ca sa nu folosesc std::swap
#define SWAP(x, y) do { auto SWAP = x; x = y; y = SWAP; } while (0)
// echivalent cu std::pair(operatori diferiti)
// il voi folosi sa sortez la final
template <typename A, typename B> struct Pair {
  A first;
  B second;
  Pair() : first(), second() {}

  Pair(const Pair &a) {
    first = a.first;
    second = a.second;
  }
  Pair(const A &a, const B &b) {
    first = A(a);
    second = B(b);
  }
  // operatorii sunt diferiti de cei de la pair normal
  // pt ca sortarea este altfel
  bool operator>(const Pair &T) {
    if (first > T.first)
      return true;
    if (first == T.first && second > T.second)
      return true;
    return false;
  }
  bool operator<(const Pair &T) {
    if (first < T.first)
      return true;
    if (first == T.first && second > T.second)
      return true;
    return false;
  }
};

bool has_number(const std::string &s) {  // daca s are numar
  std::string a = "0123456789";
  if (a.find(s[0]) != std::string::npos)  // daca primul element e cifra
    return true;
  return false;
}

// structura pt a memora scorurile fiecarui nod(player)
struct Node {
  int index, shooter, explorer, final_shooter, final_explorer;
  std::vector<int> sensors;  // senzorii pe care ii atinge
  explicit Node(int i) {
    index = i;
    shooter = 0;
    explorer = 0;
    final_shooter = 0;
    final_explorer = 0;
  }
  Node() {
    shooter = 0;
    explorer = 0;
    final_shooter = 0;
    final_explorer = 0;
  }
};

// functie de sortare
// quicksort
template <typename T> void my_sort(std::vector<T> *arr, int left, int right) {
  int i = left, j = right;
  auto pivot = (*arr)[(left + right) / 2];
  while (i <= j) {
    while ((*arr)[i] < pivot)
      i++;
    while (pivot < (*arr)[j])
      j--;
    if (i <= j) {
      SWAP((*arr)[i], (*arr)[j]);
      i++;
      j--;
    }
  }
  if (left < j)
    my_sort(arr, left, j);
  if (i < right)
    my_sort(arr, i, right);
}

// similar cu std::reverse
// ptc am pus operatorii invers si sorteaza pe dos
// si nu am stat sa ii mai schimb
template <typename T> void my_reverse(std::vector<T> *arr) {
  unsigned int n = (*arr).size() - 1;
  for (unsigned int i = 0; i <= n / 2; i++)
    SWAP((*arr)[i], (*arr)[n - i]);
}

// fiecarei laturi din graf ii asociem o variabila in care memoram de cate ori
// a fost introdusa (de cate ori s-au impuscat intre ei)
// adica graf cu greutati
struct Edge {
  int nr, count;
  explicit Edge(int x) {
    nr = x;
    count = 1;
  }
  ~Edge() {}
};

// o clasa care o sa contina ce e nevoie pt un joc intreg
class Game {
  int score[2], team1_number, team2_number, n, nr_sensors, alive_team[2];
  std::vector<std::vector<int>> sensors_map;  // matricea cu senzori
  std::vector<Node> nodes;
  std::vector<std::vector<Edge>> graph;
  std::vector<int> alive;
  std::unordered_map<std::string, int> index_map;  // pt asocierea nume->index

 public:
  Game() {
    score[0] = 0;
    score[1] = 0;
  }  // o sa aloc vectorii la citire
  bool team(const std::string &name) {
    return (!(index_map[name] < team1_number));  // true = team 1
  }
  std::string get_name(const int &index) {
  // ma plimb prin map pana gasesc indicele
    for (auto &it : index_map)
      if (it.second == index)
        return it.first;
    return NULL;
  }
  int winner() {
    if (alive_team[0] > 0)  // team 1 a castigat
      return 0;
    return 1;
  }
  void read_sensors() {  // citeste matricea de senzori
    int x;
    std::cin >> nr_sensors;
    sensors_map = std::vector<std::vector<int>>(nr_sensors);
    for (int i = 0; i < nr_sensors; i++) {
      for (int j = 0; j < nr_sensors; j++) {
        if (i == j) {
          sensors_map[i].push_back(0);
        } else {
          std::cin >> x;
          sensors_map[i].push_back(x);
        }
      }
    }
  }
  void read_names() {
    std::string s;
    std::cin >> team1_number >> team2_number;
    n = team1_number + team2_number;  // nr total de jucatori
    nodes.reserve(n);
    for (int i = 0; i < n; i++) {
      std::cin >> s;
      index_map[s] = i;  // ii asociez un indice
      nodes.push_back(Node(i));
    }
    alive = std::vector<int>(n);
    graph = std::vector<std::vector<Edge>>(n);
  }
  void new_round() {
    alive_team[0] = team1_number;
    alive_team[1] = team2_number;
    for (int i = 0; i < n; i++) {
      alive[i] = 2;
      nodes[i].shooter = nodes[i].explorer = 0;
      nodes[i].sensors.clear();  // eliberez vectorii de senzori vizitati
    }
  }

  void update_scores() {
    int a, b;
    for (int i = 0; i < n; i++) {
      for (int j = 1; j < (int)nodes[i].sensors.size(); j++) {
        a = nodes[i].sensors[j];
        b = nodes[i].sensors[j - 1];
        if (a != b)
          nodes[i].explorer += sensors_map[b][a];
      }
      // sortez si elimin duplicatele ca sa vad unique sensors
      if (nodes[i].sensors.size() > 0) {  // daca sortez ceva gol da seg fault
        my_sort<int>(&nodes[i].sensors, 0, nodes[i].sensors.size() - 1);
        int uniq = 1;
        for (unsigned int x = 1; x < nodes[i].sensors.size(); x++)
          if (nodes[i].sensors[x] != nodes[i].sensors[x - 1])
            uniq++;
        nodes[i].explorer += 3 * uniq;
      }
      // adaug scorurile / runda la scorurile finale
      nodes[i].final_explorer += nodes[i].explorer;
      nodes[i].final_shooter += nodes[i].shooter;
    }
  }

  int probable_winner() {
    float chance[2];
    chance[0] = chance[1] = 0;
    int max_shooter = -999999, max_explorer = -1;
    for (int i = 0; i < n; i++) {
      if (nodes[i].final_shooter > max_shooter)
        max_shooter = nodes[i].final_shooter;
      if (nodes[i].final_explorer > max_explorer)
        max_explorer = nodes[i].final_explorer;
    }
    float a, b, c;
    for (int i = 0; i < n; i++) {
      a = b = c = 0;
      if (max_shooter != 0)  // evit impartirea la 0
        a = (float)nodes[i].final_shooter / (float)max_shooter;
      if (max_explorer != 0)
        b = (float)nodes[i].final_explorer / (float)max_explorer;
      if (alive[i])
        c = a + b;
      if (i < team1_number)  // e in team0
        chance[0] += c;
      else  // e in team1
        chance[1] += c;
    }
    if (chance[0] >= chance[1])
      return 0;
    return 1;
  }
  void end_round() {  // pt cand se termina o runda normala
    update_scores();  // actualizez scorurile pt explorer si shooter
    // aflu cine a castigat si cresc scorul
    score[winner()]++;
  }

  void end_match() {
    update_scores();
    score[probable_winner()]++;
    // vectori pe care ii sortez si afisez
    std::vector<Pair<int, std::string>> top_shooters;
    std::vector<Pair<int, std::string>> top_explorers;
    std::vector<Pair<int, Pair<std::string, std::string>>> fire_exchange;
    int found;
    std::string t, q;
    for (auto &i : nodes) {
      t = get_name(i.index);
      Pair<int, std::string> A(i.final_shooter, t);  // nume shooter_score
      Pair<int, std::string> B(i.final_explorer, t);  // nume explorer_score
      top_shooters.push_back(A);
      top_explorers.push_back(B);
    }
    for (int i = 0; i < n; i++)
      for (int j = 0; j < (int)graph[i].size(); j++) {  // fiecare pereche
        found = 0;
        t = get_name(graph[i][j].nr);
        q = get_name(i);
        for (auto &it : fire_exchange)
          if (it.second.first == t && it.second.second == q) {
            // verific daca am adaugat perechea inversa in vector
            // gen pt Ana Mihai verific daca am adaugat Mihai Ana
            found = 1;
            it.first += graph[i][j].count;  // adaug la ce am gasit
          }
        if (found == 0) {
          Pair<std::string, std::string> C(q, t);
          Pair<int, Pair<std::string, std::string>> D(graph[i][j].count, C);
          fire_exchange.push_back(D);  // adaug in vector o pereche noua
        }
      }
    for (auto &it : fire_exchange)
      if (it.second.first > it.second.second)
        SWAP(it.second.first,
                  it.second.second);  // sortez numele in cadrul perechilor
    my_sort<Pair<int, std::string>>(&top_shooters, 0, top_shooters.size() - 1);
    my_sort<Pair<int, std::string>>(&top_explorers, 0,
     top_explorers.size() - 1);
    my_sort<Pair<int, Pair<std::string, std::string>>>(
         &fire_exchange, 0, fire_exchange.size() - 1);
    // am facut sortarea invers si fac reverse
    my_reverse<Pair<int, std::string>>(&top_shooters);
    my_reverse<Pair<int, std::string>>(&top_explorers);
    my_reverse<Pair<int, Pair<std::string, std::string>>>(&fire_exchange);
    int k;
    if (n < 5) {  // afisez tot
      std::cout << "I. Top shooters\n";
      for (int i = 0; i < n; i++)
        std::cout << i + 1 << ". " << top_shooters[i].second << " "
                  << top_shooters[i].first << "p\n";
      std::cout << "\n"
                << "II. Top explorers\n";
      for (int i = 0; i < n; i++)
        std::cout << i + 1 << ". " << top_explorers[i].second << " "
                  << top_explorers[i].first << "p\n";
    } else {
      std::cout << "I. Top shooters\n";
      for (int i = 0; i < 5; i++)
        std::cout << i + 1 << ". " << top_shooters[i].second << " "
                  << top_shooters[i].first << "p\n";
      k = 5;
      while (k < (int)top_shooters.size() &&
             top_shooters[k].first == top_shooters[4].first) {
        std::cout << k + 1 << ". " << top_shooters[k].second << " "
                  << top_shooters[k].first << "p\n";
        k++;
      }
      std::cout << "\nII. Top explorers\n";

      for (int i = 0; i < 5; i++)
        std::cout << i + 1 << ". " << top_explorers[i].second << " "
                  << top_explorers[i].first << "p\n";
      k = 5;
      while (k < (int)top_explorers.size() &&
             top_explorers[k].first == top_explorers[4].first) {
        std::cout << k + 1 << ". " << top_explorers[k].second << " "
                  << top_explorers[k].first << "p\n";
        k++;
      }
    }
    std::cout << "\nIII. Top fire exchange\n";
    if ((n - 1) * n / 2 < 5) {
      for (int i = 0; i < (int)fire_exchange.size(); i++)
        std::cout << i + 1 << ". " << fire_exchange[i].second.first << " - "
                  << fire_exchange[i].second.second << " "
                  << fire_exchange[i].first << "\n";
    } else {
      for (int i = 0; i < 5; i++)
        std::cout << i + 1 << ". " << fire_exchange[i].second.first << " - "
                  << fire_exchange[i].second.second << " "
                  << fire_exchange[i].first << "\n";
      k = 5;
      while (k < (int)fire_exchange.size() &&
             fire_exchange[k].first == fire_exchange[4].first) {
        std::cout << k + 1 << ". " << fire_exchange[k].second.first << " - "
                  << fire_exchange[k].second.second << " "
                  << fire_exchange[k].first << "\n";
        k++;
      }
    }
    std::cout << "\nIV. Final score\n" << score[0] << " - " << score[1] << "\n";
  }
  void add_edge(const std::string &s1, const std::string &s2) {
    int shooter = index_map[s1], victim = index_map[s2];
    if (alive[victim] == 1)  // a murit
      alive_team[team(s2)]--;
    if (alive[victim] > 0)
      alive[victim]--;
    if (team(s1) == team(s2))
      nodes[shooter].shooter -= 5;
    else
      nodes[shooter].shooter += 2;

    int found = 0;
    for (auto &x : graph[shooter])
      if (x.nr == victim) {
        found = 1;
        x.count++;
      }

    if (found == 0)
      graph[shooter].push_back(Edge(victim));  // adaug muchia
  }
  void add_sensor(int index, std::string name) {
    nodes[index_map[name]].sensors.push_back(index);
  }

  void read_game() {
    int first = 0;
    std::string a, b, c;
    int d;
    while (std::cin >> a) {  // caut sa vad ce tip de operatie este
      if (a.find("JOC") != std::string::npos) {  // joc nou
        if (first == 0) {
          first = 1;
        } else {
          end_round();
        }
        new_round();
      } else if (has_number(a)) {  // e linie de tipul senzor: nume
        a.pop_back();
        d = std::stoi(a, nullptr, 10);
        std::cin >> b;
        add_sensor(d, b);
      } else if (a.find("END_CHAMPIONSHIP") != std::string::npos) {
        end_match();
        break;
      } else {
        std::cin >> b >> c;  // b = "->"
        add_edge(a, c);
      }
    }
  }
};

int main() {
  Game A;
  A.read_sensors();
  A.read_names();
  A.read_game();
  return 0;
}
