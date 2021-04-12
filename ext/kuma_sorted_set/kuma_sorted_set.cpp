#include <ruby.h>

#include <vector>
#include <set>
#include <functional>

using ll = long long int;
using data_t = std::pair<void *, ll>;
using fn_t = std::function<bool(const data_t&, const data_t&)>;
using set_t = std::set<data_t, fn_t>;

struct KumaSortedSet : public set_t {
  private:
    VALUE _block;
    inline ll eval(VALUE val) {return NUM2LL(rb_funcall(_block, rb_intern("call"), 1, val));}
    inline data_t make_data(VALUE val) {return {(void *)val, eval(val)};}

  public:
    KumaSortedSet(fn_t f, VALUE block) : set_t(f), _block(block) {}
    VALUE b() {return _block;}

    std::pair<KumaSortedSet::iterator, bool> insert(VALUE val) {return set_t::insert(make_data(val));}
    int erase(VALUE val) {return set_t::erase(make_data(val));}
    bool count(VALUE val) {return set_t::count(make_data(val));}
    KumaSortedSet::iterator find(VALUE val) {return set_t::find(make_data(val));}
    KumaSortedSet::iterator lower_bound(VALUE val) {return set_t::lower_bound(make_data(val));}
    KumaSortedSet::iterator upper_bound(VALUE val) {return set_t::upper_bound(make_data(val));}
};

VALUE rb_mKumaLib, rb_cKumaSortedSet;

fn_t cmp = [](const data_t& d1, const data_t& d2) {
    return d1.second < d2.second;
};

struct WrapKumaSortedSet {
    KumaSortedSet *kss;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static void mark_kuma_sorted_set(void *ptr) {
    KumaSortedSet *kss = ((WrapKumaSortedSet *)ptr)->kss;
    rb_gc_mark(kss->b());
    for (const data_t& e : *kss) rb_gc_mark(VALUE(e.first));
}

static void free_kuma_sorted_set(void *ptr) {
    WrapKumaSortedSet *wkss = (WrapKumaSortedSet *)ptr;
    if (wkss->kss) delete wkss->kss;
    xfree(ptr);
}

static size_t memsize_kuma_sorted_set(const void *ptr) {
    return ((WrapKumaSortedSet *)ptr)->kss->size() * sizeof(data_t);
}

static const rb_data_type_t kuma_sorted_set_type {
    "KumaSortedSet",
    {mark_kuma_sorted_set, free_kuma_sorted_set, memsize_kuma_sorted_set,},
    0, 0,
    0,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define GetWrapKumaSortedSet(obj, wkss) \
    WrapKumaSortedSet *(wkss); \
    TypedData_Get_Struct((obj), WrapKumaSortedSet, &kuma_sorted_set_type, (wkss))

#define GetKumaSortedSet(obj, wkss, kss) \
    GetWrapKumaSortedSet((obj), (wkss)); \
    KumaSortedSet *(kss) = (wkss)->kss

static VALUE alloc_kuma_sorted_set(VALUE klass) {
    WrapKumaSortedSet *wkss;
    return TypedData_Make_Struct(klass, WrapKumaSortedSet, &kuma_sorted_set_type, wkss);
}

static VALUE init_kuma_sorted_set(VALUE self) {
    GetWrapKumaSortedSet(self, wkss);
    VALUE block = rb_block_proc();
    wkss->kss = new KumaSortedSet(cmp, block);
    return Qnil;
}

static VALUE first_kuma_sorted_set(VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    return kss->begin() == kss->end() ? Qnil : VALUE(kss->begin()->first);
}

static VALUE min_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    KumaSortedSet::iterator itr = kss->begin();
    int n = FIX2INT(val), i;
    VALUE v[n];
    for(i = 0; i < n; ++i) {
        if (itr == kss->end()) break;
        v[i] = VALUE(itr->first);
        ++itr;
    }
    return rb_ary_new4(i, v);
}

static VALUE last_kuma_sorted_set(VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    return VALUE(kss->rbegin()->first);
}

static VALUE max_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    KumaSortedSet::reverse_iterator itr = kss->rbegin();
    int n = FIX2INT(val), i;
    VALUE v[n];
    for(i = 0; i < n; ++i) {
        if (itr == kss->rend()) break;
        v[i] = VALUE(itr->first);
        ++itr;
    }
    return rb_ary_new4(i, v);
}

static VALUE each_kuma_sorted_set(VALUE self) {
    RETURN_ENUMERATOR(self, 0, 0);
    GetKumaSortedSet(self, wkss, kss);
    for (KumaSortedSet::iterator itr = kss->begin(); itr != kss->end(); ++itr) rb_yield(VALUE(itr->first));
    return self;
}

static VALUE reverse_each_kuma_sorted_set(VALUE self) {
    RETURN_ENUMERATOR(self, 0, 0);
    GetKumaSortedSet(self, wkss, kss);
    for (KumaSortedSet::reverse_iterator itr = kss->rbegin(); itr != kss->rend(); ++itr) rb_yield(VALUE(itr->first));
    return self;
}

static VALUE empty_kuma_sorted_set(VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    return kss->empty() ? Qtrue : Qfalse;
}

static VALUE size_kuma_sorted_set(VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    return LL2NUM(kss->size());
}

static VALUE clear_kuma_sorted_set(VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    kss->clear();
    return Qnil;
}

static VALUE insert_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    return kss->insert(val).second ? Qtrue : Qfalse;
}

static VALUE push_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    kss->insert(val);
    return self;
}

static VALUE push_some_kuma_sorted_set(int argc, VALUE *argv, VALUE self) {
    GetKumaSortedSet(self, wkss, kss);
    for (int i = 0; i < argc; i++) kss->insert(argv[i]);
    return Qnil;
}

static VALUE erase_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    return kss->erase(val) ? Qtrue : Qfalse;
}

static VALUE include_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    return kss->count(val) ? Qtrue : Qfalse;
}

static VALUE find_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    KumaSortedSet::iterator itr = kss->find(val);
    return itr == kss->end() ? Qnil : VALUE(itr->first);
}

static VALUE lower_bound_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    KumaSortedSet::iterator itr = kss->lower_bound(val);
    return itr == kss->end() ? Qnil : VALUE(itr->first);
}

static VALUE upper_bound_kuma_sorted_set(VALUE self, VALUE val) {
    GetKumaSortedSet(self, wkss, kss);
    KumaSortedSet::iterator itr = kss->upper_bound(val);
    return itr == kss->end() ? Qnil : VALUE(itr->first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_kuma_sorted_set(void) {
    rb_mKumaLib = rb_define_module("KumaLib");
    rb_cKumaSortedSet = rb_define_class_under(rb_mKumaLib, "KumaSortedSet", rb_cObject);
    rb_include_module(rb_cKumaSortedSet, rb_mEnumerable);

    rb_undef_alloc_func(rb_cKumaSortedSet);
    rb_define_alloc_func(rb_cKumaSortedSet, alloc_kuma_sorted_set);

    rb_define_method(rb_cKumaSortedSet, "initialize", init_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "first", first_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "min", min_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "last", last_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "max", max_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "each", each_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "reverse_each", reverse_each_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "empty?", empty_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "size", size_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "clear", clear_kuma_sorted_set, 0);
    rb_define_method(rb_cKumaSortedSet, "insert", insert_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "<<", push_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "push", push_some_kuma_sorted_set, -1);
    rb_define_method(rb_cKumaSortedSet, "erase", erase_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "include?", include_kuma_sorted_set, 1);
    rb_define_alias(rb_cKumaSortedSet, "member?", "include?");
    rb_define_method(rb_cKumaSortedSet, "find", find_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "lower_bound", lower_bound_kuma_sorted_set, 1);
    rb_define_method(rb_cKumaSortedSet, "upper_bound", upper_bound_kuma_sorted_set, 1);
}