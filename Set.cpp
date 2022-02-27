#include <cstddef>  // for what purpose?
#include <initializer_list>
#include <algorithm>  // only sake std::max

template<class T>
struct Node {
    T value;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
    size_t size = 0;
    int height = 0;
    Node() = default;
    explicit Node(const T& value) : value(value), size(1), height(1) {}
    Node(const Node<T>& other);
};

template<class T>
Node<T>::Node(const Node<T>& other) {
    value = other.value;
    left = other.left;
    right = other.right;
    parent = other.parent;
    size = other.size;
    height = other.height;
}

template<class T>
size_t getSize(Node<T>* node) {
    return (node ? node->size : 0);
}

template<class T>
int getHeight(Node<T>* node) {
    return (node ? node->height : 0);
}

template<class T>
int balanceFactor(Node<T>* node) {
    if (!node)
        return 0;
    return getHeight(node->right) - getHeight(node->left);
}

template<class T>
void updateParameters(Node<T>* node) {
    if (!node)
        return;
    node->size = getSize(node->left) + getSize(node->right) + 1;
    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
}

template<class T>
Node<T>* rotateRight(Node<T>* p) {
    Node<T>* boss = p->parent;
    Node<T>* q = p->left;
    Node<T>* B = q->right;
    p->left = B;
    if (B) {
        B->parent = p;
    }
    q->right = p;
    p->parent = q;
    updateParameters(p);
    updateParameters(q);
    q->parent = boss;
    return q;
}

template<class T>
Node<T>* rotateLeft(Node<T>* q) {
    Node<T>* boss = q->parent;
    Node<T>* p = q->right;
    Node<T>* B = p->left;
    q->right = B;
    if (B) {
        B->parent = q;
    }
    p->left = q;
    q->parent = p;
    updateParameters(q);
    updateParameters(p);
    p->parent = boss;
    return p;
}

template<class T>
Node<T>* balance(Node<T>* p) {
    updateParameters(p);
    if (balanceFactor(p) == 2) {
        if (balanceFactor(p->right) < 0) {
            p->right = rotateRight(p->right);
            updateParameters(p);
        }
        return rotateLeft(p);
    }
    if (balanceFactor(p) == -2) {
        if (balanceFactor(p->left) > 0) {
            p->left = rotateLeft(p->left);
            updateParameters(p);
        }
        return rotateRight(p);
    }
    return p;
}

template<typename T>
class Set {
public:
    class iterator;
    Set();
    template<typename Iterator>
    Set(Iterator first, Iterator last);
    Set(std::initializer_list<T> elems);
    Set(const Set& other);
    Set& operator= (const Set& other);
    ~Set();

    size_t size() const;
    bool empty() const;
    void insert(const T& elem);
    void erase(const T& elem);
    Set<T>::iterator begin() const;
    Set<T>::iterator end() const;
    Set<T>::iterator find(const T& elem) const;
    Set<T>::iterator lower_bound(const T& elem) const;
private:
    Node<T>* root;
    static Node<T>* insertIntoTree(Node<T>* node, Node<T> *prev, const T& elem);
    static Node<T>* eraseFromTree(Node<T>* node, const T& elem);
    static Node<T>* deepCopy(Node<T>* node, Node<T> *prev);
    static void deepDelete(Node<T>* node);
};

template<typename T>
class Set<T>::iterator {
public:
    iterator() : root(nullptr), node(nullptr) {}
    explicit iterator(Node<T>* node, Node<T>* root); // make it private with friend class Set
    iterator(const Set<T>::iterator& it) {
        root = it.root;
        node = it.node;
    }
    bool operator!=(Set<T>::iterator const& other) const;
    bool operator==(Set<T>::iterator const& other) const;
    T operator*() const;
    T* operator->() {
        return &(node->value);
    }
    Set<T>::iterator& operator++();
    Set<T>::iterator& operator--();
    Set<T>::iterator operator++(int);
    Set<T>::iterator operator--(int);
private:
    Node<T>* root;
    Node<T>* node;
};

template<typename T>
Set<T>::iterator::iterator(Node<T>* node_in, Node<T>* root_in) : root(root_in), node(node_in) {}

template<typename T>
bool Set<T>::iterator::operator!=(const Set<T>::iterator& other) const {
    return node != other.node;
}

template<typename T>
bool Set<T>::iterator::operator==(const Set<T>::iterator &other) const {
    return node == other.node;
}

template<typename T>
T Set<T>::iterator::operator*() const {
    return node->value;
}

template<typename T>
typename Set<T>::iterator& Set<T>::iterator::operator++() {
    if (node->right) {
        node = node->right;
        while (node->left) {
            node = node->left;
        }
    }
    else {
        bool cameFromRight = true;
        while (cameFromRight) {
            Node<T>* cameFrom = node;
            node = node->parent;
            if (!node) {
                return *this;
            }
            if (node->left == cameFrom) {
                cameFromRight = false;
            }
        }
    }
    return *this;
}

template<typename T>
typename Set<T>::iterator& Set<T>::iterator::operator--() {
    if (!node) {
        if (!root) {
            return *this;
        }
        node = root;
        while (node->right) {
            node = node->right;
        }
    }
    else if (node->left) {
        node = node->left;
        while (node->right) {
            node = node->right;
        }
    }
    else {
        bool cameFromLeft = true;
        while (cameFromLeft) {
            Node<T>* cameFrom = node;
            node = node->parent;
            if (!node) {
                return *this;
            }
            if (node->right == cameFrom) {
                cameFromLeft = false;
            }
        }
    }
    return *this;
}

template<typename T>
typename Set<T>::iterator Set<T>::iterator::operator++(int) {
    Set<T>::iterator temp(*this);
    ++(*this);
    return temp;
}

template<typename T>
typename Set<T>::iterator Set<T>::iterator::operator--(int) {
    Set<T>::iterator temp(*this);
    --(*this);
    return temp;
}

template<class T>
Set<T>::Set() : root(nullptr) {}

template<class T>
size_t Set<T>::size() const {
    return getSize(root);
}

template<class T>
bool Set<T>::empty() const {
    return !root;
}

template<class T>
Node<T>* Set<T>::insertIntoTree(Node<T> *node, Node<T> *prev, const T &elem) {
    if (!node) {
        node = new Node<T>(elem);
        node->parent = prev;
        return node;
    }
    if (elem < node->value) {
        node->left = insertIntoTree(node->left, node, elem);
    }
    else if (node->value < elem) {
        node->right = insertIntoTree(node->right, node, elem);
    }
    else {
        return node;
    }
    return balance(node);
}

template<class T>
void Set<T>::insert(const T &elem) {
    root = Set<T>::insertIntoTree(root, nullptr, elem);
}

template<class T>
template<typename Iterator>
Set<T>::Set(Iterator first, Iterator last) : root(nullptr) {
    for (Iterator it = first; it != last; ++it) {
        this->insert(*it);
    }
}

template<class T>
Set<T>::Set(std::initializer_list<T> elems) : Set(elems.begin(), elems.end()) {}

template<class T>
Node<T>* findMinInTree(Node<T>* root) {
    if (!root)
        return nullptr;
    Node<T>* node = root;
    while (node->left) {
        node = node->left;
    }
    return node;
}

template<class T>
Node<T>* oblivionMin(Node<T>* node) {
    if (!node->left) {
        if (node->right) {
            node->right->parent = node->parent;
        }
        return node->right;
    }
    node->left = oblivionMin(node->left);
    return balance(node);
}

template<class T>
Node<T> *Set<T>::eraseFromTree(Node<T> *node, const T &elem) {
    if (!node) {
        return nullptr;
    }
    if (elem < node->value) {
        node->left = eraseFromTree(node->left, elem);
    }
    else if (node->value < elem) {
        node->right = eraseFromTree(node->right, elem);
    }
    else {
        Node<T>* left = node->left;
        Node<T>* right = node->right;
        Node<T>* parent = node->parent;
        delete node;
        if (!right && !left) {
            return nullptr;
        }
        if (!right) {
            left->parent = parent;
            return left;
        }
        Node<T>* minNode = findMinInTree(right);
        if (left) {
            left->parent = minNode;
        }
        right->parent = minNode;
        right = oblivionMin(right);
        minNode->right = right;
        minNode->left = left;
        minNode->parent = parent;
        return balance(minNode);
    }
    return balance(node);
}

template<class T>
void Set<T>::erase(const T &elem) {
    root = eraseFromTree(root, elem);
}

template<class T>
typename Set<T>::iterator Set<T>::begin() const {
    if (!root) {
        return Set<T>::iterator(nullptr, nullptr);
    }
    Node<T>* begin = root;
    while (begin->left) {
        begin = begin->left;
    }
    return Set<T>::iterator(begin, root);
}

template<class T>
typename Set<T>::iterator Set<T>::end() const {
    return Set<T>::iterator(nullptr, root);
}

template<class T>
typename Set<T>::iterator Set<T>::find(const T &elem) const {
    Node<T>* node = root;
    while (node) {
        if (elem < node->value) {
            node = node->left;
        }
        else if (node->value < elem) {
            node = node->right;
        }
        else {
            break;
        }
    }
    return Set<T>::iterator(node, root);
}

template<class T>
typename Set<T>::iterator Set<T>::lower_bound(const T &elem) const {
    Node<T>* node = root;
    Node<T>* lastLeftTurn = nullptr;
    while (node) {
        if (elem < node->value) {
            lastLeftTurn = node;
            node = node->left;
        }
        else if (node->value < elem) {
            node = node->right;
        }
        else {
            break;
        }
    }
    if (!node) {
        return Set<T>::iterator(lastLeftTurn, root);
    }
    return Set<T>::iterator(node, root);
}

template<class T>
Node<T> *Set<T>::deepCopy(Node<T> *node, Node<T> *boss) {
    if (!node) {
        return nullptr;
    }
    Node<T> *newNode = new Node<T>(*node);
    newNode->left = deepCopy(node->left, newNode);
    newNode->right = deepCopy(node->right, newNode);
    newNode->parent = boss;
    return newNode;
}

template<class T>
Set<T>::Set(const Set &other) {
    root = deepCopy(other.root, nullptr);
}

template<class T>
void Set<T>::deepDelete(Node<T> *node) {
    if (!node) {
        return;
    }
    deepDelete(node->left);
    deepDelete(node->right);
    delete node;
}

template<class T>
Set<T>& Set<T>::operator=(const Set &other) {
    if (this == &other) {
        return *this;
    }
    deepDelete(root);
    root = deepCopy(other.root, nullptr);
    return *this;
}

template<class T>
Set<T>::~Set() {
    deepDelete(root);
}
