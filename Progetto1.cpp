#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>
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

class DSU{

    private:
        vector<int> parent;
        vector<int> rankv;
    
    public:
        DSU(int n){
            parent.resize(n);
            rankv.assign(n, 0);
            for(int i = 0; i<n; i++){
                parent[i] = i;
            }
        }

        int Find(int x){
            if(parent[x] == x) return x;
            return parent[x] = Find(parent[x]);
        }

        bool Union(int a, int b){
            a = Find(a);
            b = Find(b);
            if(a == b) return false;
            if(rankv[a]<rankv[b]) swap(a, b);
            parent[b] = a;
            if(rankv[a]==rankv[b]) rankv[a]++;
            return true;
        }
};

class Grafo{

private:
    vector<Nodo> nodi;
    vector<vector<Arco>> adj;
    unordered_map<int,int> idToIndex;
    vector<vector<Arco>> mst;
    vector<int> depth;
    vector<vector<int>> ancestor;
    vector<vector<int>> maxEdge;
    int LOG;
    
public:

    Grafo(){ LOG = 0; }
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
    
    bool trovato = false;

        for (auto& arco : adj[u]) { 
            if (arco.destinazione == v) { 
                arco.peso++; 
                break; 
            } 
        }
        if(trovato){
        for (auto& arco : adj[v]) { 
            if (arco.destinazione == u) { 
                arco.peso++; 
                break; 
            } 
        }
        return;
    }
    adj[u].push_back(Arco(v, 1));
    adj[v].push_back(Arco(u, 1));
        
}

    int NumeroNodi() const{
        return nodi.size();
}

    int NumeroArchi() const{
        long long totale = 0;
        for(const auto& lista : adj){
            totale = totale + lista.size();
        }
    return totale/2;
}

    bool ContieneNodo(int AS) const{
        return idToIndex.find(AS) != idToIndex.end();
}

    void LeggiFile(const string& nomeFile){
        ifstream file(nomeFile);
        if(!file){
            cerr << "Errore apertura file " << nomeFile << endl;

            return;
        }
        string riga;

        while(getline(file, riga)){
            size_t primoSpazio = riga.find(' ');
            if(primoSpazio == string::npos) continue;
            size_t secondoSpazio = riga.find(' ', primoSpazio+1);
            if(secondoSpazio == string::npos) continue;
            string percorso = riga.substr(primoSpazio+1, secondoSpazio-primoSpazio-1);
            replace(percorso.begin(), percorso.end(), '|', ' ');
            stringstream ss(percorso);
            vector<int> cammino;
            int as;
            while(ss >> as){
                cammino.push_back(as);
            }
            if(cammino.size() < 2) continue;
            for(size_t i= 0; i+1<cammino.size(); i++){
                AggiungiArco(cammino[i], cammino[i+1]);
            }
        }
        file.close();
    }

    void DFSComponente(int u, vector<bool>& visitato, vector<int>& componente){
        visitato[u] = true;
        componente.push_back(u);
        for(const auto& arco : adj[u]){
            int v = arco.destinazione;
            if(!visitato[v]){
                DFSComponente(v, visitato, componente);
            }
        }
    }

    vector<int> CompConnMax(){
        vector<bool> visitato(nodi.size(), false);
        vector<int> migliore;
        for(int u = 0; u<(int)nodi.size(); u++){
            if(visitato[u]) continue;
            vector<int> corrente;
            DFSComponente(u, visitato, corrente);
            cout << "Componente trovata: " << corrente.size() << endl; 
            if(corrente.size() > migliore.size()) migliore = corrente;
        }
        return migliore;
    }

    void AggiungiArcoPesato(int AS1, int AS2, int peso){
    AggiungiNodo(AS1);
    AggiungiNodo(AS2);
    int u = idToIndex[AS1];
    int v = idToIndex[AS2];

    adj[u].push_back(Arco(v,peso));

    adj[v].push_back(Arco(u,peso));
}

    Grafo Sottografo(const vector<int>& componente){
        Grafo risultato;
        vector<bool> presente(nodi.size(), false);
        for(int indice : componente){
            presente[indice] = true;
            risultato.AggiungiNodo(nodi[indice].AS);
        }
        for(int u : componente){
            for(const auto& arco : adj[u]){
                int v = arco.destinazione;
                if(!presente[v]) continue;
                if(u<v){
                    for(int k = 0; k<arco.peso; k++){
                        risultato.AggiungiArcoPesato(nodi[u].AS, nodi[v].AS, arco.peso);
                    }
                }
            }
        }
        return risultato;
    }

    void CostruisciMST(){
        vector<tuple<int, int, int>> edges;
        int n = nodi.size();
        for(int u = 0; u<n; u++){
            for(const auto& arco : adj[u]){
                int v = arco.destinazione;
                if(u<v){
                    edges.push_back({u, v, arco.peso});
                }
            }
        }
        cout << "edges.size() " << edges.size() << endl;
        sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
            return get<2>(a) < get<2>(b);
        });
        mst.assign(n, vector<Arco>());
        DSU dsu(n);
        for(const auto& e : edges){
            int u = get<0>(e);
            int v = get<1>(e);
            int w = get<2>(e);
            if(dsu.Union(u, v)){
                mst[u].push_back(Arco(v, w));
                mst[v].push_back(Arco(u, w));
            }
        }
    }

    void DFSBuild(int u, int parent, int peso){
        ancestor[0][u] = parent;
        maxEdge[0][u] = peso;
        for(const auto& arco : mst[u]){
            int v = arco.destinazione;
            if(v == parent) continue;
            depth[v] = depth[u] + 1;
            DFSBuild(v, u, arco.peso);
        }
    }

    void BinaryLifting(){
        int n = nodi.size();
        if(n == 0) return;
        LOG =1;
        while((1<<LOG) <= n) LOG++;
        ancestor.assign(LOG, vector<int>(n, -1));
        maxEdge.assign(LOG, vector<int>(n, 0));
        depth.assign(n, 0);
        DFSBuild(0, -1, 0);
        for(int k=1; k<LOG; k++){
            for(int v= 0; v<n; v++){
                int p = ancestor[k-1][v];
                if(p == -1) continue;
                ancestor[k][v] = ancestor[k-1][p];
                maxEdge[k][v] = max(maxEdge[k-1][v], maxEdge[k-1][p]);
            }
        }
    }

    int Minimax(int AS1, int AS2){
        if(!ContieneNodo(AS1) || !ContieneNodo(AS2)){
            return -1;
        }

        int u = idToIndex.at(AS1);
        int v = idToIndex.at(AS2);
        int answer = 0;

        if(depth[u] < depth[v])
            swap(u,v);

        int diff = depth[u] - depth[v];

        for(int k=LOG-1;k>=0;k--){
            if(diff & (1<<k)){
            answer = max(answer, maxEdge[k][u]);
            u = ancestor[k][u];
            }
        }

        if(u==v) return answer;

        for(int k=LOG-1;k>=0;k--){
            if(ancestor[k][u] != ancestor[k][v]){
                answer = max(answer, maxEdge[k][u]);
                answer = max(answer, maxEdge[k][v]);
                u = ancestor[k][u];
                v = ancestor[k][v];
            }
        }

        answer = max(answer, maxEdge[0][u]);
        answer = max(answer, maxEdge[0][v]);
    return answer;
}
};

int main()
{
    Grafo G;

    string nomeFile;

    cout
        << "Nome file: ";

    cin
        >> nomeFile;

    G.LeggiFile(nomeFile);

    cout
        << "Nodi letti: "
        << G.NumeroNodi()
        << endl;

    cout
        << "Archi letti: "
        << G.NumeroArchi()
        << endl;

    vector<int> lcc =
        G.CompConnMax();

    cout
        << "Dimensione LCC: "
        << lcc.size()
        << endl;
    cout << "Estrazione sottografo..." << endl;
    Grafo Gmax =
        G.Sottografo(lcc);
    cout << "Estrazione completata." << endl;
    cout
        << "Costruzione MST..."
        << endl;

    Gmax.CostruisciMST();

    cout
        << "Binary Lifting..."
        << endl;

    Gmax.BinaryLifting();

    cout
        << "Pronto."
        << endl;

    while(true)
    {
        int AS1;
        int AS2;

        cout
            << "\nAS1 AS2 (-1 -1 per uscire): ";

        cin
            >> AS1
            >> AS2;

        if(
            AS1==-1
            &&
            AS2==-1
        )
            break;

        int risposta =
            Gmax.Minimax(
                AS1,
                AS2
            );

        if(risposta==-1)
        {
            cout
                << "Nodo non presente."
                << endl;
        }
        else
        {
            cout
                << "Costo minimax = "
                << risposta
                << endl;
        }
    }

    return 0;
}