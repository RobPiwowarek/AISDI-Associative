#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class TreeMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = const value_type &;

        class ConstIterator;

        class Iterator;

        using iterator = Iterator;
        using const_iterator = ConstIterator;

        class Node;

        Node *root;
        int length;

        TreeMap() {
            root = nullptr;
            length = 0;
        }

        TreeMap(std::initializer_list<value_type> list):TreeMap() {

            for (auto it = list.begin(); it != list.end(); ++it) {
                this->operator[](it->first) = it->second;
            }
        }

        TreeMap(const TreeMap &other):TreeMap() {

            for (auto it = other.cbegin(); it != other.cend(); ++it){
                this->operator[](it->first) = it->second;
            }
        }

        void swapTree(TreeMap<KeyType, ValueType> &a, TreeMap<KeyType, ValueType> &b){
            std::swap(a.length, b.length);
            std::swap(a.root, b.root);
        }

        ~TreeMap(){
            int size = length;
            for (int i = 0; i < size; ++i){
                remove(root);
            }
        }

        TreeMap(TreeMap &&other): TreeMap() {
            swapTree(*this, other);
        }

        TreeMap &operator=(TreeMap other) {
            swapTree(*this, other);
            return *this;
        }

        bool isEmpty() const {
            return length == 0;
        }

        mapped_type &operator[](const key_type &key) {
            Node * temp = root;
            Node * parent = nullptr;

            while(temp != nullptr)
            {
                parent = temp;
                if (key == temp->getValueType().first) return temp->getValueType().second;
                else if(key > temp->getValueType().first) temp = temp->getRightChild();
                else if(key < temp->getValueType().first) temp = temp->getLeftChild();
            }

            Node * newNode = new Node(key, mapped_type{}, parent);

            if(parent != nullptr)
            {
                if(key < parent->getValueType().first)
                {
                    parent->setLeftChild(newNode);
                }
                else
                {
                    parent->setRightChild(newNode);
                }
            }
            else
            {
                root = newNode;
            }
            ++length;

            return newNode->getValueType().second;
        }

        const mapped_type &valueOf(const key_type &key) const {
            return findNode(key)->getValueType().second;
        }

        mapped_type &valueOf(const key_type &key) {
            return findNode(key)->getValueType().second;
        }

        const_iterator find(const key_type &key) const {

            Node *temp = root;

            while (temp != nullptr) {
                if (temp->getKey() < key) {
                    temp = temp->getRightChild();
                } else if (temp->getKey() > key) {
                    temp = temp->getLeftChild();
                } else {
                    return ConstIterator(temp, this);
                }
            }

            return cend();
        }

        Node *findNode(const key_type &key) const {
            Node *temp = root;

            while (temp != nullptr) {
                if (temp->getKey() < key) {
                    temp = temp->getRightChild();
                } else if (temp->getKey() > key) {
                    temp = temp->getLeftChild();
                } else {
                    return temp;
                }
            }

            throw std::out_of_range("");
        }

        iterator find(const key_type &key) {
            Node *temp = root;

            while (temp != nullptr) {
                if (temp->getKey() < key) {
                    temp = temp->getRightChild();
                } else if (temp->getKey() > key) {
                    temp = temp->getLeftChild();
                } else {
                    return Iterator(ConstIterator(temp, this));
                }
            }

            return end();
        }

        void remove(const key_type &key) {
            Node *temp = findNode(key);
            remove(temp);
        }

        void remove(Node * temp){
            if (temp == nullptr || root == nullptr) throw std::out_of_range("");

            Node *parent = temp->getParent();
            Node *leftChild = temp->getLeftChild();
            Node *rightChild = temp->getRightChild();

            // if node has no children
            if (!temp->hasChildren()) {
                if (parent == nullptr) root = nullptr;
                else
                    if (temp == parent->getLeftChild())
                        parent->setLeftChild(nullptr);
                    else
                        parent->setRightChild(nullptr);
            } // has Left child only
            else if (leftChild != nullptr && rightChild == nullptr) {
                if (parent == nullptr) {
                    root = leftChild;
                    root->setParent(nullptr);
                }
                else {
                    if (temp == parent->getLeftChild())
                        parent->setLeftChild(leftChild);
                    else
                        parent->setRightChild(leftChild);

                    leftChild->setParent(parent);
                }
            } // has Right child only
            else if (rightChild != nullptr && leftChild == nullptr) {
                if (parent == nullptr) {
                    root = rightChild;
                    root->setParent(nullptr);
                }
                else {
                    if (temp == parent->getLeftChild())
                        parent->setLeftChild(rightChild);
                     else
                        parent->setRightChild(rightChild);

                    rightChild->setParent(parent);
                }
            } else {

                Node * successor = rightChild;

                while(successor->getLeftChild() != nullptr)
                    successor=successor->getLeftChild();

                temp->setValue(successor->getValueType());

                remove(successor);
                return;
            }

            --length;
            delete temp;
        }

        void remove(const const_iterator &it) {
            remove(it.node);
        }

        size_type getSize() const {
            return (size_type) length;
        }

        bool operator==(const TreeMap &other) const {
            if (length != other.length) return false;

            ConstIterator it1 = this->cbegin();

            ConstIterator it2 = other.cbegin();

            while (it1 != this->cend() && it2 != this->cend()){
                 if ((*it1).first != ((*it2).first) || ((*it1).second != (*it2).second)){
                     return false;
                 }
                it1++;
                it2++;
            }

            return true;
        }

        bool operator!=(const TreeMap &other) const {
            return !(*this == other);
        }

        iterator begin() {
            Node *temp = root;

            if (this->length == 0) return end();
            else {
                while (temp->getLeftChild() != nullptr) temp = temp->getLeftChild();

                return Iterator(ConstIterator(temp, this));
            }
        }

        iterator end() {
            return Iterator(ConstIterator(nullptr, this));
        }

        const_iterator cbegin() const {
            Node *temp = root;

            if (this->length == 0) return end();
            else {
                while (temp->getLeftChild() != nullptr) {
                    temp = temp->getLeftChild();
                }

                return ConstIterator(temp, this);
            }

        }

        const_iterator cend() const {
            return ConstIterator(nullptr, this);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename TreeMap::value_type;
        using pointer = const typename TreeMap::value_type *;

        const TreeMap *map;
        TreeMap::Node *node;

        explicit ConstIterator() {
            map = nullptr;
            node = nullptr;
        }

        ConstIterator(TreeMap::Node *node, const TreeMap *map) {
            this->node = node;
            this->map = map;
        }

        ConstIterator(const ConstIterator &other) {
            map = other.map;
            node = other.node;
        }

        ConstIterator &operator++() {
            if (node == nullptr) throw std::out_of_range("");

            if (node->getRightChild() != nullptr) {

                node = node->getRightChild();

                while (node->getLeftChild() != nullptr) {
                    node = node->getLeftChild();
                }

            } else {

                while (node->getParent() != nullptr && node->getParent()->getRightChild() == node) {
                    node = node->getParent();
                }

                if (node->getParent() != nullptr) {
                    node = node->getParent();
                } else {
                    node = nullptr;
                }
            }

            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator it(this->node, this->map);
            this->operator++();
            return it;
        }

        ConstIterator &operator--() {
            if (*this == map->cbegin() || map->length <= 0) throw std::out_of_range("");

            if (this->node == nullptr){
                Node * temp = map->root;
                Node * parent = nullptr;

                while (temp != nullptr){
                    parent = temp;
                    temp = temp->getRightChild();
                }

                this->node = parent;
                return *this;
            }

            if (node->getLeftChild() != nullptr) {

                node = node->getLeftChild();

                while (node->getRightChild() != nullptr) {
                    node = node->getRightChild();
                }

            } else {

                while (node->getParent() != nullptr && node->getParent()->getLeftChild() == node) {
                    node = node->getParent();
                }

                if (node->getParent() != nullptr) {
                    node = node->getParent();
                } else {
                    node = nullptr;
                }

            }

            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator it(this->node, this->map);
            this->operator--();
            return it;
        }

        reference operator*() const {
            if (!node) throw std::out_of_range("");

            return node->getValueType();
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator &other) const {
            if (this->map->getSize() != other.map->getSize()) return false;

            return node == other.node;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::reference;
        using pointer = typename TreeMap::value_type *;

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

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::Node {
        value_type *value;

        Node *left;
        Node *right;
        Node *parent;

    public:
        Node(){
            value = nullptr;
            left = right = parent = nullptr;
        }

        Node(const key_type key, mapped_type val, Node* parent): left(nullptr), right(nullptr), parent(parent)
        {
            value = new value_type(key, val);
        }

        ~Node() {
            if (value) delete value;
        }

        Node *getParent() { return parent; }

        Node *getRightChild() { return right; }

        Node *getLeftChild() { return left; }

        void setLeftChild(Node *newLeft) { left = newLeft; }

        void setRightChild(Node *newRight) { right = newRight; }

        void setParent(Node *newParent) { parent = newParent; }

        void setKey(key_type key) {
            value_type *temp;

            if (value)
                temp = new value_type(key, value->second);
            else
                temp = new value_type(key, mapped_type{});

            delete value;
            value = temp;
        }

        void setValue(value_type val) {
            value_type *temp = new value_type(val.first, val.second);
            if (value)
                delete value;

            value = temp;
        }

        bool hasChildren() { return (right != nullptr || left != nullptr); }

        key_type getKey() { return value->first; }

        value_type& getValueType() { return *value; }
    };

}

#endif /* AISDI_MAPS_MAP_H */
