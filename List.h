    #pragma once

    #include <iostream>
    #include <string>
    #include <unordered_map>
    #include <vector>

    struct ListNode {
        ListNode *prev = nullptr;
        ListNode *next = nullptr;
        ListNode *rand = nullptr;
        std::string data;
    };

    class List {
    public:
        void Serialize(FILE *file) {
            fwrite(&count, sizeof(count), 1, file);
            std::unordered_map<ListNode *, uint32_t> m;
            uint32_t last_idx = 1;
            auto addPtr = [&](auto *node) {
                uint32_t idx = 0;
                if (!node) {
                    idx = 0;
                } else if (auto it = m.find(node); it != m.end()) {
                    idx = it->second;
                } else {
                    idx = last_idx++;
                    m.emplace(node, idx);
                }
                return idx;
            };
            auto serializePtr = [&](auto *node) {
                auto idx = addPtr(node);
                fwrite(&idx, sizeof(idx), 1, file);
            };
            for (auto node = head; node; node = node->next) {
                serializePtr(node);
                addPtr(node->next);
                serializePtr(node->rand);
                auto sz = node->data.size();
                fwrite(&sz, sizeof(sz), 1, file);
                fwrite(node->data.data(), node->data.size(), 1, file);
            }
        }

        void Deserialize(FILE *file) {
            Reset();
            fread(&count, sizeof(count), 1, file);
            if (!count) {
                return;
            }
            std::vector<ListNode *> nodes(count);
            for (int i = 0; i < count; i++) {
                nodes[i] = new ListNode();
            }
            uint32_t idx_n, idx_rand;
            size_t str_size;
            for (int i = 0; i < count; i++) {
                fread(&idx_n, sizeof(idx_n), 1, file);
                fread(&idx_rand, sizeof(idx_rand), 1, file);
                fread(&str_size, sizeof(str_size), 1, file);
                auto *node = nodes[idx_n - 1];
                node->data.resize(str_size);
                fread(&node->data[0], str_size, 1, file);
                node->next = i + 1 < count ? nodes[i + 1] : nullptr;
                node->rand = idx_rand ? nodes[idx_rand - 1] : nullptr;
                node->prev = i > 0 ? nodes[i - 1] : nullptr;
            }
            head = nodes.front();
            tail = nodes.back();
        }

        ~List() { Reset(); }

        void Reset() {
            while (head) {
                auto t = head;
                head = head->next;
                delete t;
            }
            count = 0;
        }

        // debug
        void Init() {
            auto b1 = new ListNode();
            auto b2 = new ListNode();
            auto b3 = new ListNode();
            auto b4 = new ListNode();

            b1->data = "_b1";
            b1->next = b2;
            b1->rand = b3;

            b2->data = "_b2";
            b2->next = b3;
            b2->prev = b1;
            b2->rand = b4;

            b3->data = "_b3";
            b3->next = b4;
            b3->prev = b2;
            b3->rand = b1;

            b4->data = "_b4";
            b4->prev = b3;
            b4->rand = b3;

            head = b1;
            tail = b4;
            count = 4;
        }

        // debug
        void print() const {
            using std::cout;
            cout << (head ? head->data : "null") << " " << (tail ? tail->data : "null") << "\n";
            for (auto n = head; n; n = n->next) {
                cout << n->data << ": " << (n->next ? n->next->data : "null") << " " << (n->prev ? n->prev->data : "null")
                     << " " << (n->rand ? n->rand->data : "null") << "\n";
            }
        }

    private:
        ListNode *head = nullptr;
        ListNode *tail = nullptr;
        int count = 0;
    };
