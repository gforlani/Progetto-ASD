#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <algorithm>

using namespace std;

struct Arco{
    int destinazione;
    int peso;
    Arco(int d, int p) : destinazione(d), peso(p) {}
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
        for (auto& arco : adj[u]) { 
            if (arco.destinazione == v) { 
                arco.peso = it->second; 
                break; 
            } 
        }
        for (auto& arco : adj[v]) { 
            if (arco.destinazione == u) { 
                arco.peso = it->second; 
                break; 
            } 
        }
    }
    else{
    frequenze[key] = 1;
    adj[u].push_back(Arco(v));
    adj[v].push_back(Arco(u));
    }

    
}

    const vector<Arco>& Vicini(int AS) const{

    return adj[idToIndex.at(AS)];
}
    int NumeroNodi() const{

    return nodi.size();
}

bool DFS(int corr, int destinazione, int soglia, vector<bool>& visitato){
    if(corr == destinazione) return true;
    visitato[corr] = true;
    for(const Arco& arco : adj[corr]){
        if(visitato[arco.destinazione]) continue;
        if(arco.peso > soglia) continue;
        if(DFS(arco.destinazione, destinazione, soglia, visitato)) return true;
    }
    return false;
}

bool EsisteCammino(int AS1, int AS2, int soglia){
    vector<bool> visitato(nodi.size(), false);
    int sorgente = idToIndex.at(AS1);
    int destinazione = idToIndex.at(AS2);

    return DFS(sorgente, destinazione, soglia, visitato);
}

vector<int> PesiDistinti() const{
    vector<int> pesi;
    for(const auto&coppia : frequenze){
        pesi.push_back(coppia.second);
    }
    if(pesi.empty()) return pesi;
    sort(pesi.begin(), pesi.end());
    pesi.erase(unique(pesi.begin(), pesi.end()), pesi.end());
    return pesi;
}

int CostoMinimax(int AS1, int AS2){
    vector<int> pesi = PesiDistinti();
    int left = 0;
    int right = pesi.size() - 1;
    int risposta = -1;
    while(left <= right){
        int mid = left + (right-left)/2;
        int soglia = pesi[mid];
        if(EsisteCammino(AS1, AS2, soglia)){
            risposta = soglia;
            right = mid - 1;
        }
        else{
            left = mid + 1;
        }
    }
    return risposta;
}

};