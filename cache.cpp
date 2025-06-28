#include <iostream>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

const int CACHE_SIZE = 32 * 1024;
const int BLOCK_SIZE = 64;
const int ASSOCIATIVITY = 8;
const int NUM_SETS = CACHE_SIZE / (BLOCK_SIZE * ASSOCIATIVITY);
const int ADDRESS_BITS = 40;
const int WORD_SIZE = 4;

int hitCount = 0;
int missCount = 0;

struct CacheLine {
    bool isValid;
    uint64_t tag;
    int lruCounter;
    vector<int> data;

    CacheLine() : isValid(false), tag(0), lruCounter(0), data(BLOCK_SIZE / WORD_SIZE, 0) {}
};

class CacheSystem {
private:
    vector<vector<CacheLine>> cacheSets;
    mt19937 rng;

public:
    CacheSystem() : cacheSets(NUM_SETS, vector<CacheLine>(ASSOCIATIVITY)), rng(42) {}

    void resetCache() {
        for (auto& set : cacheSets) {
            for (auto& line : set) {
                line.isValid = false;
                line.tag = 0;
                line.lruCounter = 0;
                fill(line.data.begin(), line.data.end(), 0);
            }
        }
        hitCount = 0;
        missCount = 0;
    }

    bool accessCache(uint64_t address) {
        auto decodedAddress = decodeAddress(address);
        int setIdx = decodedAddress.first;
        uint64_t tag = decodedAddress.second;

        for (CacheLine& line : cacheSets[setIdx]) {
            if (line.isValid && line.tag == tag) {
                hitCount++;
                line.lruCounter = 0;
                updateLRUCounters(setIdx, &line);
                cout << "Cache Hit on address " << address << " (set " << setIdx << ", tag " << tag << ")" << endl;
                return true;
            }
        }

        missCount++;
        cout << "Cache Miss on address " << address << " (set " << setIdx << ", tag " << tag << ")" << endl;
        addLineToSet(setIdx, tag);
        return false;
    }

private:
    pair<int, uint64_t> decodeAddress(uint64_t address) {
        uint64_t tag = address >> (6 + 6);
        int setIdx = (address >> 6) & (NUM_SETS - 1);
        return {setIdx, tag};
    }

    void addLineToSet(int setIdx, uint64_t tag) {
        vector<CacheLine>& set = cacheSets[setIdx];

        CacheLine* lruLine = nullptr;
        int maxLRUCounter = -1;
        for (CacheLine& line : set) {
            if (!line.isValid) { 
                line.isValid = true;
                line.tag = tag;
                line.lruCounter = 0;
                updateLRUCounters(setIdx, &line);
                cout << "Filling empty line with tag " << tag << " in set " << setIdx << endl;
                return;
            }
            if (line.lruCounter > maxLRUCounter) {
                maxLRUCounter = line.lruCounter;
                lruLine = &line;
            }
        }

        lruLine->tag = tag;
        lruLine->isValid = true;
        lruLine->lruCounter = 0;
        updateLRUCounters(setIdx, lruLine);
        cout << "Replacing LRU line in set " << setIdx << " with tag " << tag << endl;
    }

    void updateLRUCounters(int setIdx, CacheLine* accessedLine) {
        for (CacheLine& line : cacheSets[setIdx]) {
            if (&line != accessedLine) {
                line.lruCounter++;
            }
        }
    }
};

void simulateCache(CacheSystem& cache, int numAccesses) {
    mt19937 rng(42);
    uniform_int_distribution<uint64_t> addressDist(0, NUM_SETS * BLOCK_SIZE * ASSOCIATIVITY - 1);

    for (int i = 0; i < numAccesses / 2; i++) {
        uint64_t address = (i % NUM_SETS) * BLOCK_SIZE;
        cache.accessCache(address);
    }

    for (int i = numAccesses / 2; i < numAccesses; i++) {
        uint64_t address = addressDist(rng);
        cache.accessCache(address);
    }
}

int main() {
    CacheSystem cache;
    cache.resetCache();

    int numAccesses = 1000;
    simulateCache(cache, numAccesses);

    cout << "Total Cache Hits: " << hitCount << ", Total Cache Misses: " << missCount << endl;
    double hitRate = (static_cast<double>(hitCount) / (hitCount + missCount)) * 100;
    cout << "Cache Hit Rate: " << hitRate << "%" << endl;

    return 0;
}
