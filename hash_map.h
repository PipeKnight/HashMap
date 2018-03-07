#include <iostream>
#include <algorithm>
#include <list>
#include <stdexcept>
#include <vector>

using namespace std;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>> class HashMap {
public:
    using iterator = typename list<pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename list<pair<const KeyType, ValueType>>::const_iterator;
private:
    Hash hash;
    list<pair<const KeyType, ValueType>> data;
    vector<iterator> pointers{};
    size_t real_sz = 0;
    size_t buckets = 0;
    vector<size_t> sz;
    const int START = 4;
public:
    Hash hash_function() const {
        return hash;
    }
    HashMap(Hash h = Hash()): hash(h) {}
    template <class Iter> HashMap(Iter begin, Iter end, Hash h = Hash()): hash(h) {
        while (begin != end)
            insert(*begin++);
    }
    HashMap(const HashMap &other): hash(other.hash) {
        hash = other.hash;
        auto c = other.begin();
        while (c != other.end())
            insert(*c++);
    }
    HashMap(initializer_list<pair<const KeyType, ValueType>> init, Hash h = Hash()): hash(h) {
        for (auto i: init)
            insert(i);
    }
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            hash = other.hash;
            for (auto i: other)
                insert(i);
            return *this;
        }
        return *this;
    }
    iterator begin() {
        return data.begin();
    }
    const_iterator begin() const {
        return data.begin();
    }
    iterator end() {
        return data.end();
    }
    const_iterator end() const {
        return data.end();
    }
    size_t size() const {
        return real_sz;
    }
    bool empty() const {
        return real_sz == 0;
    }
    iterator find(const KeyType k) {
        if (buckets == 0) return data.end();
        auto key = hash(k) % buckets;
        auto st = pointers[key];
        for (size_t i = 0; i < sz[key]; ++i, ++st)
            if (st->first == k) return st;
        return data.end();
    }
    const_iterator find(const KeyType k) const {
        if (buckets == 0) return data.end();
        auto key = hash(k) % buckets;
        auto st = pointers[key];
        for (size_t i = 0; i < sz[key]; ++i, ++st)
            if (st->first == k) return st;
        return data.end();
    }
    void insert(pair<const KeyType, ValueType> p) {
        if (buckets == 0) rebuild();
        if (find(p.first) != data.end()) return;
        auto key = hash(p.first) % buckets;
        if (sz[key] == 0) {
            ++real_sz;
            data.insert(data.end(), p);
            pointers[key] = data.end();
            pointers[key]--;
            sz[key] = 1;
        } else {
            ++real_sz;
            ++sz[key];
            data.insert(pointers[key], p);
            pointers[key]--;
        }
        if (real_sz * 2 >= buckets) rebuild();
    }
    void erase(const KeyType k) {
        auto it = find(k);
        if (it == data.end()) return;
        auto key = hash(k) % buckets;
        --sz[key];
        --real_sz;
        if (sz[key] == 0) pointers[key] = data.end();
        else if (it == pointers[key]) ++pointers[key];
        data.erase(it);
    }
    void rebuild() {
        if (buckets == 0) {
            pointers.resize(START);
            sz.resize(START);
            buckets = START;
        } else {
            real_sz = 0;
            buckets *= 2;
            sz.clear();
            sz.resize(buckets);
            pointers.clear();
            pointers.resize(buckets);
            list<pair<const KeyType, ValueType>> ndata;
            swap(ndata, data);
            for (auto i: ndata)
                insert(i);
            ndata.clear();
        }
    }
    ValueType& operator[](const KeyType k) {
        auto it = find(k);
        if (it != data.end()) return it->second;
        insert({k, ValueType()});
        return find(k)->second;
    }
    const ValueType& at(const KeyType k) const {
        auto it = find(k);
        if (it == data.end()) throw out_of_range("OUT OF RANGE");
        return it->second;
    }
    void clear() {
        sz.clear();
        pointers.clear();
        data.clear();
        real_sz = 0;
        buckets = 0;
    }
};