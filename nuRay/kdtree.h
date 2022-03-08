#ifndef KDTREE_H
#define KDTREE_H

#include "vec3.h"

template <typename T>
struct KDNode
{
    int axis = -1;
    vec3 pos = 0.0f;
    T *data = nullptr;
    KDNode *ch[2] = {nullptr, nullptr};
};

template <typename T>
class KDTree
{
    KDNode<T> *root = nullptr;
    typedef std::pair<vec3, T *> Record;

public:
    KDTree() {}
    void build(const std::vector<Record> &elements);
    std::vector<T *> kNN(const vec3 &pos, int k) const;
};

template <typename T>
void KDTree<T>::build(const std::vector<Record> &elements)
{
    std::function<KDNode<T> *(std::vector<Record>)> solve = [&](std::vector<Record> elements) -> KDNode<T> *
    {
        if (elements.size() == 0)
            return nullptr;
        vec3 mx = elements[0].first, mn = elements[0].first;
        for (auto &i : elements)
        {
            mx = max(mx, i.first);
            mn = min(mn, i.first);
        }
        vec3 tmp = mx - mn;

        KDNode<T> *node = new KDNode<T>;
        node->axis = tmp.argmax();

        // Add some random
        if (rand() % 3 == 0)
            node->axis = rand() % 3;

        auto cmp = [&](const Record &a, const Record &b)
        {
            return a.first[node->axis] < b.first[node->axis];
        };

        sort(elements.begin(), elements.end(), cmp);

        int mid = elements.size() / 2;
        node->pos = elements[mid].first;
        node->data = elements[mid].second;
        node->ch[0] = solve(std::vector<Record>(elements.begin(), elements.begin() + mid));
        node->ch[1] = solve(std::vector<Record>(elements.begin() + mid + 1, elements.end()));
        return node;
    };

    root = solve(elements);
}

template <typename T>
std::vector<T *> KDTree<T>::kNN(const vec3 &pos, int k) const
{
    auto cmp = [&](const Record &a, const Record &b)
    {
        float dist_a = (a.first - pos).norm2();
        float dist_b = (b.first - pos).norm2();
        return dist_a < dist_b;
    };

    auto eval = [&](const Record &a) -> float
    {
        return (a.first - pos).norm2();
    };
    std::priority_queue<Record, std::vector<Record>, decltype(cmp)> record_queue(cmp);

    auto pushHeap = [&](const Record &record)
    {
        record_queue.push(record);
        if (record_queue.size() > k)
        {
            record_queue.pop();
        }
    };

    auto currentBound = [&]() -> float
    {
        if (record_queue.empty())
            return 1e38f;
        return eval(record_queue.top());
    };

    auto dist2Box = [&](vec3 pos, vec3 p0, vec3 p1) -> float
    {
        vec3 q = pos;
        q = min(q, p1);
        q = max(q, p0);
        return (q - pos).norm2();
    };

    std::function<void(KDNode<T> *, vec3, vec3)>
        solve = [&](KDNode<T> *p, vec3 p0, vec3 p1) -> void
    {
        if (p == nullptr)
            return;
        float cost = dist2Box(pos, p0, p1);
        if (cost > currentBound())
        {
            return;
        }
        pushHeap({p->pos, p->data});
        vec3 pl0 = p0, pr0 = p0, pl1 = p1, pr1 = p1;
        pl1[p->axis] = p->pos[p->axis];
        pr0[p->axis] = p->pos[p->axis];
        solve(p->ch[0], pl0, pl1);
        solve(p->ch[1], pr0, pr1);
    };

    std::function<void(KDNode<T> *, vec3, vec3)>
        find = [&](KDNode<T> *p, vec3 p0, vec3 p1) -> void
    {
        if (p == nullptr)
            return;
        pushHeap({p->pos, p->data});
        vec3 pl0 = p0, pr0 = p0, pl1 = p1, pr1 = p1;
        pl1[p->axis] = p->pos[p->axis];
        pr0[p->axis] = p->pos[p->axis];
        if (pos[p->axis] < p->pos[p->axis])
        {
            find(p->ch[0], pl0, pl1);
            solve(p->ch[1], pr0, pr1);
        }
        else
        {
            find(p->ch[1], pr0, pr1);
            solve(p->ch[0], pl0, pl1);
        }
    };

    find(root, -1e18, 1e18);

    std::vector<T *> ans;
    while (!record_queue.empty())
    {
        ans.push_back(record_queue.top().second);
        record_queue.pop();
    }

    return ans;
}

#endif // KDTREE_H
