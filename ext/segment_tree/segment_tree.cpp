#include <ruby.h>

#include <vector>
#include <functional>

using ll = long long int;
using data_t = std::pair<const void *, ll>;
#define vec(T) std::vector<T>
using fn_t = std::function<ll(VALUE)>;
using bfn_t = std::function<bool(VALUE)>;

// https://github.com/atcoder/ac-library/blob/master/atcoder/segtree.hpp
class SegTree {
  private:
    int _n, _size, _log;
    data_t _e; // 初期値
    vec(data_t) _d;
    VALUE _block;

    inline ll eval(VALUE obj) const {return NUM2LL(rb_funcall(_block, rb_intern("call"), 1, obj));}
    inline data_t make_data(VALUE obj) const {return {(void *)obj, eval(obj)};}
    inline const data_t& cmp(const data_t& d1, const data_t& d2) const {return d1.second > d2.second ? d1 : d2;}
    inline void update(int k) {_d[k] = cmp(_d[2 * k], _d[2 * k + 1]);}

  public:
    SegTree(VALUE ary, VALUE e, VALUE block) : _n(rb_array_len(ary)), _block(block) {
        _e = make_data(e);
        _log = 0;
        while ((1U << _log) < (unsigned int)(_n)) _log++;
        _size = 1 << _log;
        _d = vec(data_t)(2 * _size, _e);

        for (int i = 0; i < _n; i++) _d[_size + i] = make_data(rb_ary_entry(ary, i));
        for (int i = _size - 1; i >= 1; i--) update(i);
    }

    void set(int p, VALUE x) {
        p += _size;
        _d[p] = make_data(x);
        for (int i = 1; i <= _log; i++) update(p >> i);
    }

    const VALUE get(int p) const {return VALUE(_d[p + _size].first);}

    const VALUE all_prod() const {return VALUE(_d[1].first);}
    const VALUE prod(int l, int r) {
        data_t sml = _e, smr = _e;
        l += _size;
        r += _size;

        while (l < r) {
            if (l & 1) sml = cmp(sml, _d[l++]);
            if (r & 1) smr = cmp(_d[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return (VALUE)(cmp(sml, smr).first);
    } 

    int max_right(int l, VALUE val) const {
        ll val_e = eval(val);
        bfn_t f = [&](ll d) {return d <= val_e;};
        if (l == _n) return _n;
        l += _size;
        data_t sm = _e;

        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(cmp(sm, _d[l]).second)) {
                while (l < _size) {
                    l *= 2;
                    if (f(cmp(sm, _d[l]).second)) {
                        sm = cmp(sm, _d[l]);
                        l++;
                    }
                }
                return l - _size;
            }
            sm = cmp(sm, _d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    int min_left(int r, VALUE val) const {
        ll val_e = eval(val);
        bfn_t f = [&](ll d) {return d > val_e;};
        if (r == 0) return 0;
        r += _size;
        data_t sm = _e;

        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(cmp(_d[r], sm).second)) {
                while (r < _size) {
                    r = (2 * r + 1);
                    if (f(cmp(_d[r], sm).second)) {
                        sm = cmp(_d[r], sm);
                        r--;
                    }
                }
                return r + 1 - _size;
            }
            sm = cmp(_d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

    void mark() const {
        rb_gc_mark(_block);
        for (int i = 0; i < _d.size(); i++) rb_gc_mark(VALUE(_d[i].first));
    }
};

VALUE rb_mKumaLib, rb_cSegTree;

struct WrapSegTree {
    SegTree* st;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static void mark_seg_tree(void *ptr) {
    ((WrapSegTree *)ptr)->st->mark();
}

static void free_seg_tree(void *ptr) {
    WrapSegTree *wpq = (WrapSegTree *)ptr;
    if (wpq->st) delete wpq->st;
    xfree(ptr);
}

static size_t memsize_seg_tree(const void *ptr) {
    return sizeof(*((WrapSegTree *)ptr)->st);
}

static const rb_data_type_t seg_tree_type {
    "SegmentTree",
    {mark_seg_tree, free_seg_tree, memsize_seg_tree,},
    0, 0,
    0,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define GetWrapSegTree(obj, wst) \
    WrapSegTree *(wst); \
    TypedData_Get_Struct((obj), WrapSegTree, &seg_tree_type, (wst))

#define GetSegTree(obj, wst, st) \
    GetWrapSegTree((obj), (wst)); \
    SegTree *(st) = (wst)->st

static VALUE alloc_seg_tree(VALUE klass) {
    WrapSegTree *wst = nullptr;
    return TypedData_Make_Struct(klass, WrapSegTree, &seg_tree_type, wst);
}

static VALUE init_seg_tree(VALUE self, VALUE ary, VALUE e) {
    GetWrapSegTree(self, wst);
    VALUE block = rb_block_proc();
    wst->st = new SegTree(ary, e, block);
    return Qnil;
}

static VALUE set_seg_tree(VALUE self, VALUE pos, VALUE val) {
    GetSegTree(self, wst, st);
    st->set(FIX2INT(pos), val);
    return Qnil;
}

static VALUE get_seg_tree(VALUE self, VALUE pos) {
    GetSegTree(self, wst, st);
    return st->get(FIX2INT(pos));
}

static VALUE all_prod_seg_tree(VALUE self) {
    GetSegTree(self, wst, st);
    return st->all_prod();
}

static VALUE prod_seg_tree(VALUE self, VALUE l, VALUE r) {
    GetSegTree(self, wst, st);
    return st->prod(FIX2INT(l), FIX2INT(r));
}

static VALUE max_right_seg_tree(VALUE self, VALUE l, VALUE val) {
    GetSegTree(self, wst, st);
    return INT2FIX(st->max_right(FIX2INT(l), val));
}

static VALUE min_left_seg_tree(VALUE self, VALUE r, VALUE val) {
    GetSegTree(self, wst, st);
    return INT2FIX(st->min_left(FIX2INT(r), val));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_segment_tree() {
    rb_mKumaLib = rb_define_module("KumaLib");
    rb_cSegTree = rb_define_class_under(rb_mKumaLib, "SegmentTree", rb_cObject);

    rb_undef_alloc_func(rb_cSegTree);
    rb_define_alloc_func(rb_cSegTree, alloc_seg_tree);

    rb_define_method(rb_cSegTree, "initialize", init_seg_tree, 2);
    rb_define_method(rb_cSegTree, "set", set_seg_tree, 2);
    rb_define_method(rb_cSegTree, "get", get_seg_tree, 1);
    rb_define_method(rb_cSegTree, "all_prod", all_prod_seg_tree, 0);
    rb_define_method(rb_cSegTree, "prod", prod_seg_tree, 2);
    // rb_define_method(rb_cSegTree, "max_right", max_right_seg_tree, 2);
    // rb_define_method(rb_cSegTree, "min_left", min_left_seg_tree, 2);
}