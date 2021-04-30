#include <algorithm>
#include <limits>
#include <vector>

#include <ruby.h>

namespace atcoder {
// https://github.com/atcoder/ac-library/blob/master/atcoder/internal_queue.hpp
namespace internal {
template <class T> struct simple_queue {
    std::vector<T> payload;
    int pos = 0;
    void reserve(int n) { payload.reserve(n); }
    int size() const { return int(payload.size()) - pos; }
    bool empty() const { return pos == int(payload.size()); }
    void push(const T& t) { payload.push_back(t); }
    T& front() { return payload[pos]; }
    void clear() { payload.clear(); pos = 0; }
    void pop() { pos++; }
}; // class simple_queue
}  // namespace internal

// https://github.com/atcoder/ac-library/blob/master/atcoder/maxflow.hpp
struct mf_graph {
  private:
    using Cap = long long;
    int _n;
    struct _edge {
        int to, rev;
        Cap cap;
    };
    using _vedge = std::vector<_edge>;
    std::vector<std::pair<int, int> > pos;
    std::vector<_vedge> g;

  public:
    mf_graph() : _n(0) {}
    explicit mf_graph(int n) : _n(n), g(n) {}

    int add_edge(int from, int to, Cap cap) {
        if (from < 0 || _n <= from) rb_raise(rb_eArgError, "Graph don't have node %d.\n", from);
        if (to < 0 || _n <= to) rb_raise(rb_eArgError, "Graph don't have node %d.\n", to);
        if (cap < 0) rb_raise(rb_eArgError, "Edge capacity must be 0 or more (given %lld).\n", cap);

        int m = int(pos.size());
        pos.push_back({from, int(g[from].size())});

        int from_id = int(g[from].size());
        int to_id = int(g[to].size());
        if (from == to) to_id++;
        g[from].push_back(_edge{to, to_id, cap});
        g[to].push_back(_edge{from, from_id, 0});

        return m;
    }

    struct edge {
        int from, to;
        Cap cap, flow;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        if (i < 0 || m <= i) rb_raise(rb_eArgError, "Graph don't have edge %d.\n", i);

        _edge _e = g[pos[i].first][pos[i].second];
        _edge _re = g[_e.to][_e.rev];
        return edge{pos[i].first, _e.to, _e.cap + _re.cap, _re.cap};
    }

    std::vector<edge> edges() {
        int m = int(pos.size());
        std::vector<edge> result;
        for (int i = 0; i < m; i++) result.push_back(get_edge(i));
        return result;
    }

    void change_edge(int i, Cap new_cap, Cap new_flow) {
        int m = int(pos.size());
        if (i < 0 || m <= i) rb_raise(rb_eArgError, "Graph don't have edge %d.\n", i);
        if (new_cap < 0) rb_raise(rb_eArgError, "Edge capacity must be 0 or more (given %lld).\n", new_cap);
        if (new_cap < new_flow) rb_raise(rb_eArgError, "Edge flow must be the capacity %lld or less (given %lld).\n", new_cap, new_flow)

        _edge& _e = g[pos[i].first][pos[i].second];
        _edge& _re = g[_e.to][_e.rev];
        _e.cap = new_cap - new_flow;
        _re.cap = new_flow;
    }

    Cap flow(int s, int t) {
        return flow(s, t, std::numeric_limits<Cap>::max());
    }

    Cap flow(int s, int t, Cap flow_limit) {
        if (s < 0 || _n <= s) rb_raise(rb_eArgError, "Invalid first Argment. (method is flow(%d, %d, %lld))", s, t, flow_limit);
        if (t < 0 || _n <= t) rb_raise(rb_eArgError, "Invalid second Argment. (method is flow(%d, %d, %lld))", s, t, flow_limit);
        if (s == t) rb_raise(rb_eArgError, "Invalid first Argment don't equal to second Argment. (method is flow(%d, %d, %lld))", s, t, flow_limit);

        std::vector<int> level(_n), iter(_n);
        internal::simple_queue<int> que;

        auto bfs = [&]() {
            std::fill(level.begin(), level.end(), -1);
            level[s] = 0;
            que.clear();
            que.push(s);
            while (!que.empty()) {
                int v = que.front();
                que.pop();
                for (auto e : g[v]) {
                    if (e.cap == 0 || level[e.to] >= 0) continue;
                    level[e.to] = level[v] + 1;
                    if (e.to == t) return;
                    que.push(e.to);
                }
            }
        };

        auto dfs = [&](auto self, int v, Cap up) {
            if (v == s) return up;
            Cap res = 0;
            int level_v = level[v];
            for (int& i = iter[v]; i < int(g[v].size()); i++) {
                _edge& e = g[v][i];
                if (level_v <= level[e.to] || g[e.to][e.rev].cap == 0) continue;
                Cap d =
                    self(self, e.to, std::min(up - res, g[e.to][e.rev].cap));
                if (d <= 0) continue;
                g[v][i].cap += d;
                g[e.to][e.rev].cap -= d;
                res += d;
                if (res == up) return res;
            }
            level[v] = _n;
            return res;
        };

        Cap flow = 0;
        while (flow < flow_limit) {
            bfs();
            if (level[t] == -1) break;
            std::fill(iter.begin(), iter.end(), 0);
            Cap f = dfs(dfs, t, flow_limit - flow);
            if (!f) break;
            flow += f;
        }
        return flow;
    }

    std::vector<bool> min_cut(int s) {
        std::vector<bool> visited(_n);
        internal::simple_queue<int> que;
        que.push(s);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            visited[p] = true;
            for (const _edge& e : g[p]) {
                if (e.cap && !visited[e.to]) {
                    visited[e.to] = true;
                    que.push(e.to);
                }
            }
        }
        return visited;
    }
}; // class mf_graph
}  // namespace atcoder

////////////////////////////////////////////////////////////////////////////////////////////////////
// rubyオブジェクトのための準備

namespace kuma_lib {
namespace max_flow {
    struct WrapMfGraph {
        atcoder::mf_graph *mf;
    }; // ラッパークラス

    static void mark(void *ptr) {} // 内部にVALUEがないから何もしない

    // オブジェクトの開放
    static void free(void *ptr) {
        WrapMfGraph *wmf = (WrapMfGraph *)ptr;
        if (wmf->mf) delete wmf->mf;
        xfree(ptr);
    }

    // オブジェクトのサイズ
    static size_t memsize(const void *ptr) {
        return sizeof(*(((WrapMfGraph *)ptr)->mf));
    }

    // オブジェクトのデータタイプ
    static const rb_data_type_t mf_graph_type {
        "MaxFlow",
        {mark, free, memsize,},
        0, 0,
        0,
    };

////////////////////////////////////////////////////////////////////////////////////////////////////
// rubyが呼び出す関数の定義

    // ゲッター1
    #define GetWrapMfGraph(obj, wmf) \
        WrapMfGraph *(wmf); \
        TypedData_Get_Struct((obj), WrapMfGraph, &mf_graph_type, (wmf))

    // ゲッター2
    #define GetMfGraph(obj, wmf, mf) \
        GetWrapMfGraph((obj), (wmf)); \
        atcoder::mf_graph *(mf) = (wmf)->mf

    static VALUE alloc(VALUE klass) {
        WrapMfGraph *wmf = nullptr;
        return TypedData_Make_Struct(klass, WrapMfGraph, &mf_graph_type, wmf);
    }

    static VALUE initialize(VALUE self, VALUE n) {
        GetWrapMfGraph(self, wmf);
        wmf->mf = new atcoder::mf_graph(NUM2INT(n));
        return Qnil;
    }

    // add directed edge
    static VALUE add_edge(VALUE self, VALUE from, VALUE to, VALUE cap) {
        GetWrapMfGraph(self, wmf);
        return INT2NUM(wmf->mf->add_edge(NUM2INT(from), NUM2INT(to), NUM2LL(cap)));
    }

    // edge -> ruby array
    static VALUE edge2ary(atcoder::mf_graph::edge e) {
        return rb_ary_new3(4, INT2NUM(e.from), INT2NUM(e.to), LL2NUM(e.cap), LL2NUM(e.flow));
    }

    // edges -> ruby array
    static VALUE edges2ary(std::vector<atcoder::mf_graph::edge> es) {
        int i = es.size();
        VALUE elts[i];
        for(int j = 0; j < i; ++j) elts[j] = edge2ary(es[j]);
        return rb_ary_new4(i, elts);
    }

    static VALUE get_edge(VALUE self, VALUE n) {
        GetMfGraph(self, wmf, mf);
        return edge2ary(mf->get_edge(NUM2INT(n)));
    }

    static VALUE edges(VALUE self) {
        GetMfGraph(self, wmf, mf);
        return edges2ary(mf->edges());
    }

    static VALUE change_egde(VALUE self, VALUE i, VALUE new_cap, VALUE new_flow) {
        GetMfGraph(self, wmf, mf);
        mf->change_edge(NUM2INT(i), NUM2LL(new_cap), NUM2LL(new_flow));
        return Qnil;
    }

    static VALUE flow(int argc, VALUE *argv, VALUE self) {
        if (argc < 2 || 3 < argc) rb_raise(rb_eArgError, "Argument is not correct.");
        GetMfGraph(self, wmf, mf);
        int source = NUM2INT(argv[0]), sink = NUM2INT(argv[1]);
        if (argc == 2) return LL2NUM(mf->flow(source, sink));
        else return LL2NUM(mf->flow(source, sink, NUM2LL(argv[2])));
    }

    static VALUE min_cat(VALUE self, VALUE source) {
        GetMfGraph(self, wnf, mf);
        std::vector<bool> visited = mf->min_cut(NUM2INT(source));
        int l = visited.size();
        VALUE elts[l];
        for (int i = 0; i < l; ++i) elts[i] = visited[i] ? Qtrue : Qfalse;
        return rb_ary_new4(l, elts);
    }

}; // namespace max_flow
}; // namespace kuma_lib

extern "C" void Init_max_flow() {
    VALUE rb_mKumaLib = rb_define_module("KumaLib");
    VALUE rb_cMaxFlow = rb_define_class_under(rb_mKumaLib, "MaxFlow", rb_cObject);

    rb_undef_alloc_func(rb_cMaxFlow);
    rb_define_alloc_func(rb_cMaxFlow, kuma_lib::max_flow::alloc);

    rb_define_method(rb_cMaxFlow, "initialize", kuma_lib::max_flow::initialize, 1);
    rb_define_method(rb_cMaxFlow, "add_edge", kuma_lib::max_flow::add_edge, 3);
    rb_define_method(rb_cMaxFlow, "get_edge", kuma_lib::max_flow::get_edge, 1);
    rb_define_method(rb_cMaxFlow, "edges", kuma_lib::max_flow::edges, 0);
    rb_define_method(rb_cMaxFlow, "change_edge", kuma_lib::max_flow::change_egde, 3);
    rb_define_method(rb_cMaxFlow, "flow", kuma_lib::max_flow::flow, -1);
    rb_define_method(rb_cMaxFlow, "min_cut", kuma_lib::max_flow::min_cat, 1);
}