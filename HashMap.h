#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class HashMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = const value_type &;

        class ConstIterator;

        class Iterator;

        class BucketNode;

        static const int BUCKETS = 10;

        BucketNode ** buckets;
        int *sizes;

        using iterator = Iterator;
        using const_iterator = ConstIterator;

        HashMap() {
            sizes = new int[BUCKETS]();
            buckets = new BucketNode*[BUCKETS]();
        }

        ~HashMap(){
                for (int i = 0; i < BUCKETS; ++i) {
                    BucketNode *temp = buckets[i];
                    BucketNode *temp2;

                    if (!buckets[i]) continue;

                    while (temp != nullptr) {
                        temp2 = temp->next;
                        delete temp;
                        temp = temp2;
                    }
                }

                delete [] buckets;
                delete [] sizes;
        }

        HashMap(std::initializer_list<value_type> list): HashMap() {
                for (auto it = list.begin(); it != list.end(); ++it) {
                    this->operator[](it->first) = it->second;
                }
        }

        void swapMap(HashMap<KeyType, ValueType> &a, HashMap<KeyType, ValueType> &b) {
            std::swap(a.buckets, b.buckets);
            std::swap(a.sizes, b.sizes);
        }

        HashMap(const HashMap &other): HashMap() {
            for (auto it = other.begin(); it != other.end(); ++it){
                this->operator[](it->first) = it->second;
            }
        }

        HashMap(HashMap &&other):HashMap() {
            swapMap(*this, other);
        }

        HashMap &operator=(HashMap other) {
            swapMap(*this, other);
            return *this;
        }

        bool isEmpty() const {
            return getSize() == 0;
        }


        mapped_type &operator[](const key_type &key) {
            int index = bucketHash(key);

            BucketNode * node = new BucketNode();
            delete node->val;
            node->val = new value_type(key, ValueType{});
            node->next = nullptr;

            if (!buckets[index]){
                buckets[index] = node;
                node->prev = nullptr;
                ++sizes[index];
            }
            else {
                BucketNode * temp = buckets[index];
                BucketNode * temp2 = temp;

                while (temp != nullptr){

                    if (temp->val->first == key){
                        delete node;
                        return temp->val->second;
                    }

                    temp2 = temp;
                    temp = temp->next;
                }

                temp2->next = node;
                node->prev = temp2;
                ++sizes[index];
            }

            return node->val->second;
        }

        int bucketHash(const key_type &key) const {
            return (int)std::hash<key_type >{}(key) % BUCKETS;
        }

        const mapped_type &valueOf(const key_type &key) const {
            return find(key)->second;
        }

        mapped_type &valueOf(const key_type &key) {
            return find(key)->second;
        }

        const_iterator find(const key_type &key) const {
            int index = bucketHash(key);

            if (!buckets[index]) return cend();

            BucketNode *node = buckets[index];

            for (int i = 0; i < sizes[index]; ++i){
                if (node->val->first == key) return ConstIterator(this, index, node);

                node = node->next;
            }

            return cend();
        }

        iterator find(const key_type &key) {
            int index = bucketHash(key);

            if (!buckets[index]) return end();

            BucketNode *node = buckets[index];

            for (int i = 0; i < sizes[index]; ++i){
                if (buckets[index][i].val->first == key) return Iterator(ConstIterator(this, index, node));

                node = node->next;
            }

            return end();
        }

        BucketNode * findNext(int listIndex) const {
            for (int i = listIndex; i < BUCKETS; ++i){
                if (buckets[i] != nullptr) return buckets[i];
            }

            return nullptr;
        }

        BucketNode * findPrev(int listIndex) const {
            for (int i = listIndex; i >= 0; --i){
                if (buckets[i] != nullptr) {
                    BucketNode*temp = buckets[i];

                    for (int j = 0; j < sizes[i]; ++j){
                        temp = temp->next;
                    }

                    return temp;
                }
            }

            return nullptr;
        }

        BucketNode * findFirst() const{
            for (int i = 0; i < BUCKETS; ++i){
                if (buckets[i] != nullptr) return buckets[i];
            }

            return nullptr;
        }

        int findFirstListIndex() const{
            for (int i = 0; i < BUCKETS; ++i){
                if (buckets[i] != nullptr) return i;
            }

            return BUCKETS;
        }

        void remove(const key_type &key) {
            remove(find(key));
        }

        void remove(const const_iterator &it) {
            int index = bucketHash(it->first);

            if (!buckets[index]) throw std::out_of_range("");

            BucketNode *temp = it.node;

            if (temp == nullptr){
                throw std::out_of_range("");
            }

            if (temp->prev){
                temp->prev->next = temp->next;
            }

            if (!temp->prev){
                buckets[index] = temp->next;
            }

            if (temp->next){
                temp->next->prev = temp->prev;
            }

            --sizes[index];
            delete temp;
        }

        int getSize() const {
            int size = 0;
            for (int i = 0; i < BUCKETS; ++i){
                size += sizes[i];
            }

            return size;
        }

        bool operator==(const HashMap &other) const {
            if (getSize() != other.getSize()) return false;
            else {

                auto it = cbegin();
                auto it2 = other.cbegin();

                while (it != cend() && it2 != other.cend()) {
                    if (it->first != it2->first || it->second != it2->second) return false;
                    ++it;
                    ++it2;
                }

                return it == cend() && it2 == other.cend();
            }
        }

        bool operator!=(const HashMap &other) const {
            return !operator==(other);
        }

        iterator begin() {
            return Iterator(ConstIterator(this, findFirstListIndex(), findFirst()));
        }

        iterator end() {
            return Iterator(ConstIterator(this, BUCKETS, nullptr));
        }

        const_iterator cbegin() const {
            return ConstIterator(this, findFirstListIndex(),findFirst());
        }

        const_iterator cend() const {
            return ConstIterator(this, BUCKETS, nullptr);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename HashMap::value_type;
        using pointer = const typename HashMap::value_type *;

        const HashMap *map;
        int listIndex = 0;
        BucketNode * node;

        explicit ConstIterator() {
            map = nullptr;
        }

        ConstIterator(const ConstIterator &other) {
            map = other.map;
            listIndex = other.listIndex;
            node = other.node;
        }

        ConstIterator(const HashMap *map, int listInd, BucketNode * node) {
            this->map = map;
            this->listIndex = listInd;
            this->node = node;
        }

        ConstIterator &operator++() {
            if (listIndex >= map->BUCKETS) throw std::out_of_range("");

            if (!node->next){
                for ( ++listIndex; listIndex < BUCKETS; ++listIndex){
                    if (map->buckets[listIndex] != nullptr) {
                        node = map->buckets[listIndex];
                        return *this;
                    }
                }

                node = nullptr;
            }
            else {
                node = node->next;
            }

            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator it(this->map, this->listIndex, this->node);

            this->operator++();

            return it;
        }

        ConstIterator &operator--() {
             if (this->node == map->findFirst()) throw std::out_of_range("");

            if (!node || !node->prev){
                for (--listIndex; listIndex >= 0; --listIndex){
                    if (map->buckets[listIndex] != nullptr) {
                        BucketNode*temp = map->buckets[listIndex];

                        while (temp->next != nullptr){
                            temp = temp->next;
                        }

                        node = temp;
                        return *this;
                    }
                }
            }
            else {
                node = node->prev;
            }

            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator it(this->map, this->listIndex, this->node);

            this->operator--();

            return it;
        }

        reference operator*() const {
            if (!node) throw std::out_of_range("");

            return *(node->val);
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator &other) const {
            if (listIndex != other.listIndex || map != other.map) return false;

            return node == other.node;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::reference;
        using pointer = typename HashMap::value_type *;

        explicit Iterator() {}

        Iterator(const ConstIterator &other)
                : ConstIterator(other) {}

        Iterator &operator++() {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator &operator--() {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        reference operator*() const {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };

    template <typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::BucketNode{
    public:
        value_type * val;

        BucketNode * next;
        BucketNode * prev;

        BucketNode(){
            val = new value_type(KeyType{}, ValueType{});
            next = nullptr;
            prev = nullptr;
        }

        ~BucketNode(){
            delete val;
        }

    };

}

#endif /* AISDI_MAPS_HASHMAP_H */
