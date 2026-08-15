#pragma once

#include <array>
#include <string>
#include <vector>

namespace trie_lib
{
    // ALPHABET: 分岐の数（英小文字なら26、01-trie(XOR用)なら2、数字なら10 など）
    // BASE    : 文字から添字への変換に使う基準文字（'a' なら c - 'a'）
    template <int ALPHABET = 26, char BASE = 'a'>
    class Trie
    {
    public:
        struct Node
        {
            std::array<int, ALPHABET> children;
            bool is_end = false;
            // このノードが表す接頭辞を持つ文字列の本数（= このノードを通過した文字列の本数）
            long long prefix_count = 0;

            // children を -1 (子なし) で初期化する。これを忘れると
            // 未初期化値が根(0)などの実在ノード番号と誤認識されてバグる。
            Node() { children.fill(-1); }
        };

        // nodes_[0] を根として初期化する
        Trie() { nodes_.push_back(Node()); }

        // ノード数の目安が事前に分かっている場合、再割り当てを避けるために呼ぶ
        // （目安 = 挿入する文字列の長さの総和 + 1）
        void reserve(int n) { nodes_.reserve(n + 1); }

        // 文字列 s を挿入し、終端ノードの番号を返す
        int insert(const std::string &s)
        {
            int cur = 0;
            for (char c : s)
            {
                const int idx = static_cast<int>(c - BASE);
                if (nodes_[cur].children[idx] == -1)
                {
                    nodes_[cur].children[idx] = static_cast<int>(nodes_.size());
                    nodes_.push_back(Node());
                }
                cur = nodes_[cur].children[idx];
                ++nodes_[cur].prefix_count;
            }
            nodes_[cur].is_end = true;
            return cur;
        }

        // 文字列 s を辿った先のノード番号を返す（途中で辿れなくなったら -1）
        int find_node(const std::string &s) const
        {
            int cur = 0;
            for (char c : s)
            {
                const int idx = static_cast<int>(c - BASE);
                if (nodes_[cur].children[idx] == -1)
                    return -1;
                cur = nodes_[cur].children[idx];
            }
            return cur;
        }

        // 文字列 s がちょうど挿入されているか（末端まで一致し、is_end が true か）
        bool contains(const std::string &s) const
        {
            const int cur = find_node(s);
            return cur != -1 && nodes_[cur].is_end;
        }

        // s を接頭辞に持つ文字列が 1 つ以上挿入されているか
        bool has_prefix(const std::string &s) const
        {
            return find_node(s) != -1;
        }

        // ノード番号からノードの中身を参照する
        // （s を1文字ずつ辿りながら nodes_[...].prefix_count を見る、
        //   といった問題ごとの探索ロジックはこれを使って呼び出し側で書く）
        const Node &node(int i) const { return nodes_[i]; }

        int root() const { return 0; }
        int size() const { return static_cast<int>(nodes_.size()); }

    private:
        std::vector<Node> nodes_;
    };
} // namespace trie_lib
