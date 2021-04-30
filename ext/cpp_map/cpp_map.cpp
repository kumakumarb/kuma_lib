#include <ruby.h>

#include <vector>
#include <map>
#include <functional>

namespace kuma_lib {
namespace cpp_map {
    using mymap_t = std::map<long long int, void *>;

    struct WrapCppMap {
        mymap_t *mp;
        void *d; // default value
    };

    void mark(void *ptr) {
        mymap_t *sh = ((WrapCppMap *)ptr)->mp;
        void *d = ((WrapCppMap *)ptr)->d;

        if (sh) { // if sh != nullptr
            for (mymap_t::iterator itr = sh->begin(); itr != sh->end(); ++itr) {
                rb_gc_mark(VALUE(itr->second));
            }
        }
        
        if (d) {rb_gc_mark(VALUE(d));} // if d != nullptr, (if は必要なさげ VALUE(nullptr) == Qfalse)
    }

    void free(void *ptr) {
        mymap_t *m = ((WrapCppMap *)ptr)->mp;
        if (m) delete m;
        xfree(ptr);
    }

    size_t memsize(const void *ptr) {
        return sizeof(*(((WrapCppMap *)ptr)->mp));
    }

    static const rb_data_type_t cpp_map_type = {
        "CppMap",
        {mark, free, memsize, },
        0, 0,
        0,
    };

////////////////////////////////////////////////////////////////////////////////////////////////////

    #define GetWSH(obj, hoge) \
        WrapCppMap *(hoge); \
        TypedData_Get_Struct((obj), WrapCppMap, &cpp_map_type, (hoge))

    #define GetSH(obj, hoge, huga) \
        GetWSH((obj), (hoge)); \
        mymap_t *(huga) = (hoge)->mp

    static VALUE alloc(VALUE klass) {
        WrapCppMap *wsh;
        return TypedData_Make_Struct(klass, WrapCppMap, &cpp_map_type, wsh);
    }

    static VALUE initialize(int argc, VALUE *argv, VALUE self) {
        if (argc > 1) {
            rb_raise(rb_eArgError, "too many argments!\n");
        } else {
            GetWSH(self, wsh);
            wsh->mp = new mymap_t;
            wsh->d = (void *)Qnil;
            if (argc == 1) wsh->d = (void *)argv[0];
            return Qnil;
        }
    }

    static VALUE set_default(VALUE self, VALUE val) {
        GetWSH(self, wsh);
        wsh->d = (void *)val;
        return Qnil;
    }

    static VALUE get_default(VALUE self) {
        GetWSH(self, wsh);
        return VALUE(wsh->d);
    }

    static inline VALUE itr2VALUE(const mymap_t::iterator& itr) {return rb_assoc_new(LL2NUM(itr->first), VALUE(itr->second));}
    static inline VALUE itr2VALUE(const mymap_t::reverse_iterator& itr) {return rb_assoc_new(LL2NUM(itr->first), VALUE(itr->second));}

    static VALUE first(VALUE self) {
        GetSH(self, hoge, sh);
        if (sh->empty()) return Qnil;
        return itr2VALUE(sh->begin());
    }

    static VALUE min(int argc, VALUE *argv, VALUE self) {
        if (argc > 2) {
            rb_raise(rb_eArgError, "too many argments!\n");
        } else if (argc == 0) {
            return first(self);
        } else {
            GetSH(self, hoge, sh);
            int n = NUM2INT(argv[0]); //
            if (n < 1) return rb_ary_new();
            int i;

            mymap_t::iterator itr = sh->begin();
            VALUE v[n];
            for (i = 0; i < n; ++i) {
                if (itr == sh->end()) break; // 
                v[i] = itr2VALUE(itr);
                ++itr;
            }
            return rb_ary_new4(i, v);
        }
    }

    static VALUE last(VALUE self) {
        GetSH(self, hoge, sh);
        if (sh->empty()) return Qnil;
        return itr2VALUE(sh->rbegin());
    }

    static VALUE max(int argc, VALUE *argv, VALUE self) {
        if (argc > 2) {
            rb_raise(rb_eArgError, "too many argments!\n");
        } else if (argc == 0) {
            return last(self);
        } else {
            GetSH(self, hoge, sh);
            int n = NUM2INT(argv[0]); //
            if (n < 1) return rb_ary_new();
            int i;

            mymap_t::reverse_iterator itr = sh->rbegin();
            VALUE v[n];
            for (i = 0; i < n; ++i) {
                if (itr == sh->rend()) break; // 
                v[i] = itr2VALUE(itr);
                ++itr;
            }
            return rb_ary_new4(i, v);
        }
    }

    static VALUE each(VALUE self) {
        RETURN_ENUMERATOR(self, 0, 0);
        GetSH(self, hoge, sh);
        for (mymap_t::iterator itr = sh->begin(); itr != sh->end(); ++itr) rb_yield(itr2VALUE(itr));
        return self;
    }

    static VALUE reverse_each(VALUE self) {
        RETURN_ENUMERATOR(self, 0, 0);
        GetSH(self, hoge, sh);
        for (mymap_t::reverse_iterator itr = sh->rbegin(); itr != sh->rend(); ++itr) rb_yield(itr2VALUE(itr));
        return self;
    }

    static VALUE empty(VALUE self) {
        GetSH(self, hoge, sh);
        return sh->empty() ? Qtrue : Qfalse;
    }

    static VALUE size(VALUE self) {
        GetSH(self, hoge, sh);
        return INT2NUM(sh->size());
    }

    static VALUE clear(VALUE self) {
        GetSH(self, hoge, sh);
        sh->clear();
        return Qnil;
    }

    static VALUE insert(VALUE self, VALUE key, VALUE val) {
        GetSH(self, hoge, sh);
        return sh->insert(std::make_pair(NUM2LL(key), (void *)val)).second ? Qtrue : Qfalse; // insert できた場合 true
    }

    // 上書きする insert
    static VALUE insert_or_assign(VALUE self, VALUE key, VALUE val) {
        GetSH(self, hoge, sh);
        return sh->insert_or_assign(NUM2LL(key), (void *)val).second ? Qtrue : Qfalse; // insert した時 true
    }

    static VALUE emplace(VALUE self, VALUE key, VALUE val) {
        GetSH(self, hoge, sh);
        return sh->emplace(NUM2LL(key), (void *)val).second ? Qtrue : Qfalse; // insert した時 true
    }

    // emplace_hint
    // try_emplace

    static VALUE erase(VALUE self, VALUE key) {
        GetSH(self, hoge, sh);
        return sh->erase(NUM2LL(key)) ? Qtrue : Qfalse;
    }

    // swap
    // extract
    
    static VALUE merge(VALUE self, VALUE other) {
        GetSH(self, hoge, sh1);
        GetSH(other, huga, sh2);
        sh1->merge(*sh2);
        return Qnil;
    }

    static VALUE bracket(VALUE self, VALUE key) {
        GetSH(self, wsh, sh);
        void *val = (*sh)[NUM2LL(key)];
        return val ? VALUE(val) : VALUE(wsh->d);
    }

    static VALUE at(VALUE self, VALUE key) {
        GetSH(self, wsh, sh);
        try {
            void *val = sh->at(NUM2LL(key));
            return val ? VALUE(val) : VALUE(wsh->d); // if val != nullptr
        } catch(const std::exception& e) {
            rb_raise(rb_eIndexError, "Object doesn't have this key! (%lld)", NUM2LL(key));
        }
    }

    // count

    static VALUE find(VALUE self, VALUE key) {
        GetSH(self, hoge, sh);
        mymap_t::iterator itr = sh->find(NUM2LL(key));
        return itr == sh->end() ? Qnil : itr2VALUE(itr);
    }

    static VALUE include(VALUE self, VALUE key) {
        GetSH(self, hoge, sh);
        return sh->find(NUM2LL(key)) != sh->end() ? Qtrue : Qfalse;
    }

    // contains (c++20)
    // equal_range

    // >= l を満たす最初の要素
    static VALUE lower_bound(VALUE self, VALUE l) {
        GetSH(self, hoge, sh);
        mymap_t::iterator itr = sh->lower_bound(NUM2LL(l));
        return itr == sh->end() ? Qnil : itr2VALUE(itr);
    }

    // < l を満たす最後の要素
    static VALUE not_lower_bound(VALUE self, VALUE l) {
        GetSH(self, hoge, sh);
        mymap_t::iterator itr = sh->lower_bound(NUM2LL(l));
        return itr == sh->begin() ? Qnil : itr2VALUE(--itr);
    }

    // > r を満たす最初の要素
    static VALUE upper_bound(VALUE self, VALUE r) {
        GetSH(self, hoge, sh);
        mymap_t::iterator itr = sh->upper_bound(NUM2LL(r));
        return itr == sh->end() ? Qnil : itr2VALUE(itr);
    }

    // <= r を満たす最後の要素
    static VALUE not_upper_bound(VALUE self, VALUE r) {
        GetSH(self, hoge, sh);
        mymap_t::iterator itr = sh->upper_bound(NUM2LL(r));
        return itr == sh->begin() ? Qnil : itr2VALUE(--itr);
    }

}; // namespace cpp_map
}; // namespace kuma_lib

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_cpp_map(void) {
    VALUE rb_mKumaLib = rb_define_module("KumaLib");
    VALUE rb_cCppMap = rb_define_class_under(rb_mKumaLib, "CppMap", rb_cObject);
    rb_include_module(rb_cCppMap, rb_mEnumerable);

    rb_undef_alloc_func(rb_cCppMap);
    rb_define_alloc_func(rb_cCppMap, kuma_lib::cpp_map::alloc);

    rb_define_method(rb_cCppMap, "initialize", kuma_lib::cpp_map::initialize, -1);
    rb_define_method(rb_cCppMap, "default=", kuma_lib::cpp_map::set_default, 1);
    rb_define_method(rb_cCppMap, "default", kuma_lib::cpp_map::get_default, 0);
    rb_define_method(rb_cCppMap, "first", kuma_lib::cpp_map::first, 0);
    rb_define_method(rb_cCppMap, "min", kuma_lib::cpp_map::min, -1);
    rb_define_method(rb_cCppMap, "last", kuma_lib::cpp_map::last, 0);
    rb_define_method(rb_cCppMap, "max", kuma_lib::cpp_map::max, -1);
    rb_define_method(rb_cCppMap, "each", kuma_lib::cpp_map::each, 0);
    rb_define_method(rb_cCppMap, "reverse_each", kuma_lib::cpp_map::reverse_each, 0);
    rb_define_method(rb_cCppMap, "empty?", kuma_lib::cpp_map::empty, 0);
    rb_define_method(rb_cCppMap, "size", kuma_lib::cpp_map::size, 0);
    rb_define_method(rb_cCppMap, "clear", kuma_lib::cpp_map::clear, 0);
    rb_define_method(rb_cCppMap, "insert", kuma_lib::cpp_map::insert, 2);
    rb_define_method(rb_cCppMap, "insert!", kuma_lib::cpp_map::insert_or_assign, 2);
    rb_define_alias(rb_cCppMap, "[]=", "insert!");
    rb_define_method(rb_cCppMap, "emplace", kuma_lib::cpp_map::emplace, 2);
    rb_define_method(rb_cCppMap, "erase", kuma_lib::cpp_map::erase, 1);
    rb_define_method(rb_cCppMap, "merge!", kuma_lib::cpp_map::merge, 1);
    rb_define_method(rb_cCppMap, "[]", kuma_lib::cpp_map::bracket, 1);
    rb_define_method(rb_cCppMap, "at", kuma_lib::cpp_map::at, 1);
    rb_define_method(rb_cCppMap, "find", kuma_lib::cpp_map::find, 1);
    rb_define_method(rb_cCppMap, "include?", kuma_lib::cpp_map::include, 1);
    rb_define_alias(rb_cCppMap, "member?", "include?");
    rb_define_method(rb_cCppMap, "lower_bound", kuma_lib::cpp_map::lower_bound, 1);
    rb_define_alias(rb_cCppMap, ">=", "lower_bound");
    rb_define_method(rb_cCppMap, "not_lower_bound", kuma_lib::cpp_map::not_lower_bound, 1);
    rb_define_alias(rb_cCppMap, "<", "not_lower_bound");
    rb_define_method(rb_cCppMap, "upper_bound", kuma_lib::cpp_map::upper_bound, 1);
    rb_define_alias(rb_cCppMap, ">", "upper_bound");
    rb_define_method(rb_cCppMap, "not_upper_bound", kuma_lib::cpp_map::not_upper_bound, 1);
    rb_define_alias(rb_cCppMap, "<=", "not_upper_bound");
}