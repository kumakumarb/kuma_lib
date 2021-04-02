#include <ruby.h>

#include <set>
#include <functional>

using ll = long long int;
using data_t = std::pair<void *, ll>;
using fn_t = std::function<bool(const data_t&, const data_t&)>;
using set_t = std::set<data_t, fn_t>;

struct KumaSet : public set_t {
  private:
    VALUE _block;
    inline ll eval(VALUE val) {return NUM2LL(rb_funcall(_block, rb_intern("call"), 1, val));}
    inline data_t make_data(VALUE val) {return {(void *)val, eval(val)};}

  public:
    KumaSet(fn_t f, VALUE block) : set_t(f), _block(block) {}
    VALUE b() {return _block;}

    std::pair<KumaSet::iterator, bool> insert(VALUE val) {return set_t::insert(make_data(val));}
    int erase(VALUE val) {return set_t::erase(make_data(val));}
    bool count(VALUE val) {return set_t::count(make_data(val));}
    KumaSet::iterator find(VALUE val) {return set_t::find(make_data(val));}
    KumaSet::iterator lower_bound(VALUE val) {return set_t::lower_bound(make_data(val));}
    KumaSet::iterator upper_bound(VALUE val) {return set_t::upper_bound(make_data(val));}
};

VALUE rb_mKumaLib, rb_cKumaSet;

fn_t cmp = [](const data_t& d1, const data_t& d2) {
    return d1.second < d2.second;
};

struct WrapKumaSet {
    KumaSet *ks;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static void mark_kuma_set(void *ptr) {
    KumaSet *ks = ((WrapKumaSet *)ptr)->ks;
    rb_gc_mark(ks->b());
    for (const data_t& e : *ks) rb_gc_mark(VALUE(e.first));
}

static void free_kuma_set(void *ptr) {
    WrapKumaSet *wks = (WrapKumaSet *)ptr;
    if (wks->ks) delete wks->ks;
    xfree(ptr);
}

static size_t memsize_kuma_set(const void *ptr) {
    return ((WrapKumaSet *)ptr)->ks->size() * sizeof(data_t);
}

static const rb_data_type_t kuma_set_type {
    "KumaSet",
    {mark_kuma_set, free_kuma_set, memsize_kuma_set,},
    0, 0,
    0,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define GetWrapKumaSet(obj, wks) \
    WrapKumaSet *(wks); \
    TypedData_Get_Struct((obj), WrapKumaSet, &kuma_set_type, (wks))

static VALUE alloc_kuma_set(VALUE klass) {
    WrapKumaSet *wks;
    return TypedData_Make_Struct(klass, WrapKumaSet, &kuma_set_type, wks);
}

static VALUE init_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    VALUE block = rb_block_proc();
    wks->ks = new KumaSet(cmp, block);
    return Qnil;
}

static VALUE first_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    KumaSet::iterator itr = wks->ks->begin();
    return itr == wks->ks->end() ? Qnil : VALUE(itr->first);
}

static VALUE last_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    return VALUE(wks->ks->rbegin()->first);
}

static VALUE each_kuma_set(VALUE self) {
    RETURN_ENUMERATOR(self, 0, 0);
    GetWrapKumaSet(self, wks);
    KumaSet *ks = wks->ks;
    for (KumaSet::iterator itr = ks->begin(); itr != ks->end(); itr++) rb_yield(VALUE(itr->first));
    return self;
}

static VALUE empty_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    return wks->ks->empty() ? Qtrue : Qfalse;
}

static VALUE size_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    return LL2NUM(wks->ks->size());
}

static VALUE clear_kuma_set(VALUE self) {
    GetWrapKumaSet(self, wks);
    wks->ks->clear();
    return Qnil;
}

static VALUE insert_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    return wks->ks->insert(val).second ? Qtrue : Qfalse;
}

static VALUE push_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    wks->ks->insert(val);
    return self;
}

static VALUE push_some_kuma_set(int argc, VALUE *argv, VALUE self) {
    GetWrapKumaSet(self, wks);
    for (int i = 0; i < argc; i++) wks->ks->insert(argv[i]);
    return Qnil;
}

static VALUE erase_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    return wks->ks->erase(val) ? Qtrue : Qfalse;
}

static VALUE include_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    return wks->ks->count(val) ? Qtrue : Qfalse;
}

static VALUE find_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    KumaSet::iterator itr = wks->ks->find(val);
    return itr == wks->ks->end() ? Qnil : VALUE(itr->first);
}

static VALUE lower_bound_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    KumaSet::iterator itr = wks->ks->lower_bound(val);
    return itr == wks->ks->end() ? Qnil : VALUE(itr->first);
}

static VALUE upper_bound_kuma_set(VALUE self, VALUE val) {
    GetWrapKumaSet(self, wks);
    KumaSet::iterator itr = wks->ks->upper_bound(val);
    return itr == wks->ks->end() ? Qnil : VALUE(itr->first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_kuma_set(void) {
    rb_mKumaLib = rb_define_module("KumaLib");
    rb_cKumaSet = rb_define_class_under(rb_mKumaLib, "KumaSet", rb_cObject);
    rb_include_module(rb_cKumaSet, rb_mEnumerable);

    rb_undef_alloc_func(rb_cKumaSet);
    rb_define_alloc_func(rb_cKumaSet, alloc_kuma_set);

    rb_define_method(rb_cKumaSet, "initialize", init_kuma_set, 0);
    rb_define_method(rb_cKumaSet, "first", first_kuma_set, 0);
    rb_define_alias(rb_cKumaSet, "min", "first");
    rb_define_method(rb_cKumaSet, "last", last_kuma_set, 0);
    rb_define_alias(rb_cKumaSet, "max", "last");
    rb_define_method(rb_cKumaSet, "each", each_kuma_set, 0);
    rb_define_method(rb_cKumaSet, "empty?", empty_kuma_set, 0);
    rb_define_method(rb_cKumaSet, "size", size_kuma_set, 0);
    rb_define_method(rb_cKumaSet, "clear", clear_kuma_set, 0);
    rb_define_method(rb_cKumaSet, "insert", insert_kuma_set, 1);
    rb_define_method(rb_cKumaSet, "<<", push_kuma_set, 1);
    rb_define_method(rb_cKumaSet, "push", push_some_kuma_set, -1);
    rb_define_method(rb_cKumaSet, "erase", erase_kuma_set, 1);
    rb_define_method(rb_cKumaSet, "include?", include_kuma_set, 1);
    rb_define_alias(rb_cKumaSet, "member?", "include?");
    rb_define_method(rb_cKumaSet, "find", find_kuma_set, 1);
    rb_define_method(rb_cKumaSet, "lower_bound", lower_bound_kuma_set, 1);
    rb_define_method(rb_cKumaSet, "upper_bound", upper_bound_kuma_set, 1);
}