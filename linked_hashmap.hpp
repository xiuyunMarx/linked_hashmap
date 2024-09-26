/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class Equal = std::equal_to<Key>
    >
    class linked_hashmap {

        template<class elemType>
        class LinkedList {
        public:
            class LinkedNode {
            public:
                elemType *val;
                LinkedNode *prev, *next;

                LinkedNode() : val(nullptr), prev(nullptr), next(nullptr) {}

                explicit LinkedNode(const elemType &other) : prev(nullptr), next(nullptr) {
                    val = new elemType(other);
                }

                LinkedNode(const LinkedNode &other) : prev(nullptr), next(nullptr) {
                    val = new elemType(*(other.val));
                }

                ~LinkedNode() {
                    if (val != nullptr) {
                        delete val;
                        val = nullptr;
                    }
                }
            };

        public:
            LinkedNode *head, *tail;

            LinkedList() {
                head = new LinkedNode;
                tail = new LinkedNode;
                head->next = tail;
                tail->prev = head;
            }

            ~LinkedList() {
                clear();
                delete head;
                delete tail;
            }

            LinkedNode *insert(LinkedNode *pos, LinkedNode *cur) {
                LinkedNode *back = pos->next;
                back->prev = cur;
                pos->next = cur;
                cur->prev = pos;
                cur->next = back;
                return cur;
            }

            LinkedNode *remove(LinkedNode *pos) {
                LinkedNode *front = pos->prev;
                LinkedNode *back = pos->next;
                front->next = back;
                back->prev = front;
                pos->prev = pos->next = nullptr;
                return pos;
            }

            LinkedList(const LinkedList &other) {
                head = new LinkedNode;
                tail = new LinkedNode;
                head->next = tail;
                tail->prev = head;
                LinkedNode *cur = other.head->next;
                while (cur != other.tail) {
                    pushBack(*(cur->val));
                    cur = cur->next;
                }
            }

            LinkedList &operator=(const LinkedList &rhs) {
                if (this == &rhs)
                    return *this;

                clear();
                delete head;
                delete tail;

                head = new LinkedNode;
                tail = new LinkedNode;
                head->next = tail;
                tail->prev = head;


                LinkedNode *cur = (rhs.head)->next;
                while (cur != rhs.tail) {
                    pushBack(*(cur->val));
                    cur = cur->next;
                }

                return *this;
            }

            void clear() {
                LinkedNode *cur = head->next;
                while (cur != tail) {
                    LinkedNode *tmp = cur->next;
                    delete remove(cur);
                    cur = tmp;
                }
            }

            bool empty() const {
                return head->next == tail;
            }

            void erase(LinkedNode *pos) {
                delete remove(pos);
            }

            LinkedNode *pushBack(const elemType &val) {
                LinkedNode *newIns = new LinkedNode(val);
                insert(tail->prev, newIns);
                return newIns;
            }
        };


    public:
        typedef pair<const Key, T> value_type;

        friend class iterator;

        friend class const_iterator;

    private:
        using dataNode = typename LinkedList<value_type>::LinkedNode; //ptr to the nodes in elemTable

        using ptrNode = typename LinkedList<dataNode *>::LinkedNode; //secondary pointer , point to the actual nodes

        Hash getHash;
        Equal judgeEqual;


        LinkedList<dataNode *> *hashList;
        LinkedList<value_type> elemTable;
        size_t capacity, loadFactor;
        size_t totLength;

    public:

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend class linked_hashmap;

        private:
            dataNode *iter;
            dataNode *identity;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:

            using difference_type = std::ptrdiff_t;
            using value_type = typename linked_hashmap::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;


            iterator() {
                iter = nullptr;
                identity = nullptr;
                // TODO
            }

            iterator(const iterator &other) {
                // TODO
                iter = other.iter;
                identity = other.identity;
            }

            iterator(const_iterator other) {
                // TODO
                iter = other.iter;
                identity = other.identity;
            }

            iterator(dataNode *other, dataNode *head) {
                // TODO
                iter = other;
                identity = head;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator ret = *this;

                if (iter->next == nullptr)
                    throw invalid_iterator();

                iter = iter->next;
                return ret;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (iter->next == nullptr)
                    throw invalid_iterator();
                iter = iter->next;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ret = *this;

                if ((iter->prev)->prev == nullptr)
                    throw invalid_iterator();

                iter = iter->prev;
                return *this;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if ((iter->prev)->prev == nullptr)
                    throw invalid_iterator();

                iter = iter->prev;
                return *this;
            }


            value_type &operator*() const {
                return *(iter->val);
            }

            bool operator==(const iterator &rhs) const {
                if (iter == rhs.iter)
                    return true;
                return false;
            }

            bool operator==(const const_iterator &rhs) const {
                if (iter == rhs.iter)
                    return true;
                return false;
            }


            bool operator!=(const iterator &rhs) const {
                if (iter != rhs.iter)
                    return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (iter != rhs.iter)
                    return true;
                return false;
            }


            value_type *operator->() const noexcept {
                return (iter->val);
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            friend class linked_hashmap;

            dataNode *iter;
            dataNode *identity;
            // data members.
        public:
            const_iterator() {
                iter = nullptr;
                identity = nullptr;
            }

            const_iterator(const const_iterator &other) {
                iter = other.iter;
                identity = other.identity;
            }

            const_iterator(dataNode *other, dataNode *head) {
                // TODO
                iter = other;
                identity = head;
            }

            const_iterator(const iterator &other) {
                iter = other.iter;
                identity = other.identity;
            }

            // And other methods in iterator.
            const_iterator operator++(int) {
                const_iterator ret = *this;

                if (iter->next == nullptr)
                    throw invalid_iterator();

                iter = iter->next;
                return ret;
            }


            const_iterator &operator++() {
                if (iter->next == nullptr)
                    throw invalid_iterator();
                iter = iter->next;
                return *this;
            }


            const_iterator operator--(int) {
                const_iterator ret = *this;

                if ((iter->prev)->prev == nullptr)
                    throw invalid_iterator();

                iter = iter->prev;
                return *this;
            }


            const_iterator &operator--() {
                if ((iter->prev)->prev == nullptr)
                    throw invalid_iterator();

                iter = iter->prev;
                return *this;
            }


            value_type &operator*() const {

                return *(iter->val);
            }

            bool operator==(const iterator &rhs) const {
                if (iter == rhs.iter)
                    return true;
                return false;
            }

            bool operator==(const const_iterator &rhs) const {
                if (iter == rhs.iter)
                    return true;
                return false;
            }


            bool operator!=(const iterator &rhs) const {
                if (iter != rhs.iter)
                    return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (iter != rhs.iter)
                    return true;
                return false;
            }


            value_type *operator->() const noexcept {
                return iter->val;
            }
        };

        /**
         * TODO two constructors
         */
        void buildHashList() {
            if (hashList != nullptr)
                delete[] hashList;
            hashList = new LinkedList<dataNode *>[capacity];

            dataNode *cur;
            cur = (elemTable.head)->next;
            while (cur != elemTable.tail) {
                size_t hashVal = getHash((*(cur->val)).first);
                size_t idx = hashVal % capacity;

                hashList[idx].pushBack(cur);
                cur = cur->next;
            }

            return;
        }

        void halfSize(){
            if(capacity/2 == 0)
                return ;
            capacity/=2;
            delete [] hashList;

            hashList = new LinkedList<dataNode * > [capacity];

            dataNode  *cur = (elemTable.head)->next;

            while(cur != elemTable.tail){
                size_t hashVal = getHash((*(cur->val)).first);
                size_t idx = hashVal % capacity;

                hashList[idx].pushBack(cur);
                cur = cur->next;
            }

            return ;
        }
        void doubleSize() {
            capacity *= 2;
            buildHashList();
        }

        linked_hashmap() {
            loadFactor = 1<<5 ;
            capacity = 1 << 15;
            totLength = 0;
            hashList = new LinkedList<dataNode *>[capacity];
        }

        linked_hashmap(const linked_hashmap &other) {
            loadFactor = other.loadFactor;
            capacity = other.capacity;

            totLength = other.totLength;


            hashList = nullptr;
            elemTable = other.elemTable;
            buildHashList();

        }

        /**
         * TODO assignment operator
         */
        linked_hashmap &operator=(const linked_hashmap &other) {
            if (this == &other)
                return *this;

            delete[] hashList;
            elemTable.clear();

            capacity = other.capacity;
            loadFactor = other.loadFactor;

            totLength = other.totLength;

            hashList = nullptr;

            elemTable = other.elemTable;

            buildHashList();

            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            delete[] hashList;
            hashList = nullptr;
//            elemTable.clear();
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            iterator dataIter = find(key);
            if (dataIter == end())
                throw index_out_of_bound();

            dataNode *dataPos = dataIter.iter;

            return (*(dataPos->val)).second;
        }

        const T &at(const Key &key) const {
            const_iterator dataIter = find(key);

            if (dataIter == cend())
                throw index_out_of_bound();

            const dataNode *dataPos = dataIter.iter;

            return (*(dataPos->val)).second;

        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            size_t keyHash = getHash(key);
            size_t idx = keyHash % capacity;

            ptrNode *cur = (hashList[idx].head)->next;
            while (cur != hashList[idx].tail) {
                dataNode *dataPos = *(cur->val);
                if (judgeEqual(key, (*(dataPos->val)).first))
                    return (*(dataPos->val)).second;

                cur = cur->next;
            }


            value_type newIns(key, T());

            totLength++;
            if (totLength >= loadFactor * capacity)
                doubleSize();

            idx = keyHash % capacity;


            dataNode *dataPos = elemTable.pushBack(newIns);
            hashList[idx].pushBack(dataPos);

            return (*(dataPos->val)).second;


        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            const_iterator pos = find(key);

            if (pos != cend()) {
                const dataNode *dataPos = pos.iter;
                return (*(dataPos->val)).second;
            } else
                throw index_out_of_bound();


        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator((elemTable.head)->next, elemTable.head);
        }

        const_iterator cbegin() const {
            const_iterator ret((elemTable.head)->next, elemTable.head);
            return ret;
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            iterator ret(elemTable.tail, elemTable.head);
            return ret;
        }

        const_iterator cend() const {
            const_iterator ret(elemTable.tail, elemTable.head);
            return ret;
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            if (totLength == 0)
                return true;
            return false;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return totLength;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < capacity; i++)
                hashList[i].clear();
            elemTable.clear();
            totLength = 0;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            iterator pos = find(value.first);
            if (pos != end()) {
                pair<iterator, bool> ret(pos, false);
                return ret;
            }

            totLength++;
            if (totLength >= capacity * loadFactor)
                doubleSize();

            dataNode *dataPos = elemTable.pushBack(value);

            size_t keyHash = getHash(value.first);
            size_t idx = keyHash % capacity;

            hashList[idx].pushBack(dataPos);

            iterator ret(dataPos, elemTable.head);

            return pair<iterator, bool>(ret, true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */

        void erase(iterator pos) {
            if (pos == end())
                throw index_out_of_bound();

            if (pos.identity != elemTable.head)
                throw index_out_of_bound();

            totLength--;
            if(totLength < capacity*loadFactor / 2)
                halfSize();

            dataNode *dataPos = pos.iter;
            value_type data = *(dataPos->val);

            size_t keyHash = getHash(data.first);
            size_t idx = keyHash % capacity;

            ptrNode *cur = (hashList[idx].head)->next;
            while (cur != hashList[idx].tail) {
                if (dataPos == *(cur->val)) {
                    hashList[idx].erase(cur);
                    break;
                }
                cur = cur->next;
            }

            elemTable.erase(dataPos);
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */

        bool checkExistence(const Key KV) const {
            size_t keyHash = getHash(KV);
            size_t idx = keyHash % capacity;

            ptrNode *cur = (hashList[idx].head)->next;
            while (cur != hashList[idx].tail) {
                dataNode *tmp = *(cur->val);

                value_type data = *(tmp->val);
                if (judgeEqual(KV, data.first))
                    return true;


                cur = cur->next;
            }

            return false;
        }

        size_t count(const Key &key) const {
            if (checkExistence(key))
                return 1;
            else {
                return 0;
            }
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            size_t keyHash = getHash(key);
            size_t idx = keyHash % capacity;

            ptrNode *cur = (hashList[idx].head)->next;

            while (cur != hashList[idx].tail) {
                dataNode *dataPos = *(cur->val);
                if (judgeEqual(key, (*(dataPos->val)).first)) {
                    iterator ret(dataPos, elemTable.head);
                    return ret;
                }

                cur = cur->next;
            }

            return end();
        }


        const_iterator find(const Key &key) const {
            size_t keyHash = getHash(key);
            size_t idx = keyHash % capacity;

            ptrNode *cur = (hashList[idx].head)->next;

            while (cur != hashList[idx].tail) {
                dataNode *dataPos = *(cur->val);
                if (judgeEqual(key, (*(dataPos->val)).first)) {
                    const_iterator ret(dataPos, elemTable.head);
                    return ret;
                }

                cur = cur->next;
            }

            return cend();
        }
    };

}

#endif