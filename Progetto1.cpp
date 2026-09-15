#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;

struct Arco{
    int destinazione;
    Arco(int d) : destinazione(d) {}
};

struct Nodo{
    int AS;
    Nodo(int id) : AS(id) {}
};

class Grafo{

private:
    vector<Nodo> nodi;
    vector<vector<Arco>> adj;
    unordered_map<int,int> idToIndex;
    unordered_map<long long,int> frequenze;
    
    long long ChiaveArco(int u, int v) const{
        if(u > v)
            swap(u,v);
        return (static_cast<long long>(u) << 32)
               | static_cast<unsigned int>(v);
    }


public:

    void AggiungiNodo(int AS){

    if(idToIndex.find(AS) != idToIndex.end())
        return;
    int indice = nodi.size();

    nodi.push_back(Nodo(AS));
    adj.push_back(vector<Arco>());
    idToIndex[AS] = indice;
}

    void AggiungiArco(int AS1, int AS2){

    if(AS1 == AS2)
        return;
    AggiungiNodo(AS1);
    AggiungiNodo(AS2);
    int u = idToIndex[AS1];
    int v = idToIndex[AS2];
    long long key = ChiaveArco(u,v);
    auto it = frequenze.find(key);

    if(it != frequenze.end()){
        it->second++;
        return;
    }

    frequenze[key] = 1;
    adj[u].push_back(Arco(v));
    adj[v].push_back(Arco(u));
}

    const vector<Arco>& Vicini(int AS) const{

    int indice = idToIndex.at(AS);

    return adj[indice];
}

    int Frequenza(int AS1, int AS2) const{

    int u = idToIndex.at(AS1);
    int v = idToIndex.at(AS2);

    long long key = ChiaveArco(u,v);

    auto it = frequenze.find(key);

    if(it == frequenze.end())
        return 0;

    return it->second;
}
    int NumeroNodi() const{

    return nodi.size();
}

};
