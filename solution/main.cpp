#include <bits/stdc++.h>
#include "../lib/cpp/pugixml-1.14/src/pugixml.hpp"

using namespace std;
using namespace pugi;

struct Clan {
    string name;
    bool isMine;
    int mar = 0;  
    int rt = 0; 
    int ptr = 0;  
    int availableResources = 0;
    int refillTimeRemaining = 0; // Time left for refill
};

struct Road {
    string from;
    string to;
    int time;
};

vector<Clan> clans;
vector<Road> roads;
unordered_map<string, vector<pair<string, int>>> adjList;
unordered_map<string, int> blockedClans;
int totalGoldCaptured = 0;
int currentTime = 0; // Global time tracker

unordered_map<string, Clan*> clanMap; // Fast lookup for clans by name

void parseXML(const string& filePath) {
    xml_document doc;
    if (!doc.load_file(filePath.c_str())) {
        cerr << "Error: Failed to load XML file: " << filePath << endl;
        exit(1);
    }

    for (xml_node clan : doc.child("Kingdom").children("Clan")) {
        Clan c;
        c.name = clan.child("Name").text().as_string();
        c.isMine = string(clan.child("IS_MINE").text().as_string()) == "True";
        if (c.isMine) {
            c.mar = clan.child("MAR").text().as_int();
            c.rt = clan.child("RT").text().as_int();
            c.ptr = clan.child("PTR").text().as_int();
            c.availableResources = c.mar;
        }
        clans.push_back(c);
        clanMap[c.name] = &clans.back(); // Store reference in map
    }

    for (xml_node road : doc.child("Kingdom").children("Road")) {
        Road r;
        r.from = road.child("From").text().as_string();
        r.to = road.child("To").text().as_string();
        r.time = road.child("Time").text().as_int();
        roads.push_back(r);
        adjList[r.from].push_back({r.to, r.time});
        adjList[r.to].push_back({r.from, r.time});
    }
}

// Helper function: Find nearest mine with resources
string findNearestMine(const string& clanName) {
    if (adjList.find(clanName) == adjList.end()) return ""; // No connections

    unordered_map<string, int> minDist;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> pq;

    minDist[clanName] = 0;
    pq.push({0, clanName});

    while (!pq.empty()) {
        auto [dist, current] = pq.top(); pq.pop();
        if (clanMap[current]->isMine && clanMap[current]->availableResources > 0)
            return current; // Found nearest mine with resources

        for (auto& [neighbor, travelTime] : adjList[current]) {
            if (blockedClans[neighbor] > currentTime) continue; // Blocked
            int newDist = dist + travelTime;
            if (!minDist.count(neighbor) || newDist < minDist[neighbor]) {
                minDist[neighbor] = newDist;
                pq.push({newDist, neighbor});
            }
        }
    }
    return ""; // No available mine
}

void attackClan(const string& name, int rr, int gco) {
    if (clanMap.find(name) == clanMap.end()) return;
    
    Clan* targetClan = clanMap[name];
    int gatheredResources = 0;

    while (gatheredResources < rr) {
        string mineName = findNearestMine(name);
        if (mineName.empty()) break; // No available mines

        Clan* mine = clanMap[mineName];
        int fetchable = min(rr - gatheredResources, mine->availableResources);

        mine->availableResources -= fetchable;
        gatheredResources += fetchable;

        if (mine->availableResources == 0)
            mine->refillTimeRemaining = currentTime + mine->rt; // Set refill time
    }

    double fractionGold = ((double)gatheredResources / rr) * gco;
    totalGoldCaptured += fractionGold;
}

void showMinesStatus() {
    for (const auto& c : clans) {
        if (c.isMine) {
            cout << c.name << ": " << c.availableResources << "/" << c.mar << " available\n";
        }
    }
}

void produceGold() {
    cout << "Gold captured: " << totalGoldCaptured << endl;
}

void processQueries() {
    string query;
    while (getline(cin, query)) {
        size_t pos;
        if ((pos = query.find("Attack on")) != string::npos) {
            string clanName;
            int rr, gco;
            sscanf(query.c_str() + pos, "Attack on %s with %dRR providing %d GCO", &clanName[0], &rr, &gco);
            attackClan(clanName, rr, gco);
        } 
        else if (query.find("Show the current status of all the clans with mines") != string::npos) {
            showMinesStatus();
        } 
        else if (query.find("Produce the current amount of Gold captured") != string::npos) {
            produceGold();
        } 
        else if ((pos = query.find("Clan")) != string::npos && query.find("has found natural resource") != string::npos) {
            string clanName;
            int mar, ptr, rt;
            sscanf(query.c_str() + pos, "Clan%s has found natural resource's mine with %d MAR, %d PTR and %d RT", &clanName[0], &mar, &ptr, &rt);
            if (clanMap.find(clanName) != clanMap.end()) {
                Clan* c = clanMap[clanName];
                c->isMine = true;
                c->mar = mar;
                c->ptr = ptr;
                c->rt = rt;
                c->availableResources = mar;
            }
        } 
        else if ((pos = query.find("Clan")) != string::npos && query.find("has been blocked") != string::npos) {
            string clanName;
            int blockTime;
            sscanf(query.c_str() + pos, "Clan%s has been blocked by enemies for %d seconds", &clanName[0], &blockTime);
            blockedClans[clanName] = currentTime + blockTime;
        } 
        else if (query.find("Victory of Codeopia") != string::npos) {
            break;
        }

        currentTime++; // Increment global time for refill tracking
        for (auto& c : clans) {
            if (c.isMine && c.refillTimeRemaining == currentTime) {
                c.availableResources = c.mar; // Refill mine
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <XML file path>" << endl;
        return 1;
    }

    string xmlFilePath = argv[1]; 
    parseXML(xmlFilePath);

    processQueries();
    return 0;
}
