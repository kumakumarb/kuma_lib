#include <ruby.h>

#include <vector>
#include <map>
#include <functional>

namespace kuma_lib {
namespace cpp {
namespace map {
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

}; // namespace map
}; // namespace cpp
}; // namespace kuma_lib

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_map(void) {
    VALUE rb_mKumaLib = rb_define_module("KumaLib");
    VALUE rb_mCpp = rb_define_module_under(rb_mKumaLib, "Cpp");
    VALUE rb_cMap = rb_define_class_under(rb_mCpp, "Map", rb_cObject);
    rb_include_module(rb_cMap, rb_mEnumerable);

    rb_undef_alloc_func(rb_cMap);
    rb_define_alloc_func(rb_cMap, kuma_lib::cpp::map::alloc);

    rb_define_method(rb_cMap, "initialize", kuma_lib::cpp::map::initialize, -1);
    rb_define_method(rb_cMap, "default=", kuma_lib::cpp::map::set_default, 1);
    rb_define_method(rb_cMap, "default", kuma_lib::cpp::map::get_default, 0);
    rb_define_method(rb_cMap, "first", kuma_lib::cpp::map::first, 0);
    rb_define_method(rb_cMap, "min", kuma_lib::cpp::map::min, -1);
    rb_define_method(rb_cMap, "last", kuma_lib::cpp::map::last, 0);
    rb_define_method(rb_cMap, "max", kuma_lib::cpp::map::max, -1);
    rb_define_method(rb_cMap, "each", kuma_lib::cpp::map::each, 0);
    rb_define_method(rb_cMap, "reverse_each", kuma_lib::cpp::map::reverse_each, 0);
    rb_define_method(rb_cMap, "empty?", kuma_lib::cpp::map::empty, 0);
    rb_define_method(rb_cMap, "size", kuma_lib::cpp::map::size, 0);
    rb_define_method(rb_cMap, "clear", kuma_lib::cpp::map::clear, 0);
    rb_define_method(rb_cMap, "insert", kuma_lib::cpp::map::insert, 2);
    rb_define_method(rb_cMap, "insert!", kuma_lib::cpp::map::insert_or_assign, 2);
    rb_define_alias(rb_cMap, "[]=", "insert!");
    rb_define_method(rb_cMap, "emplace", kuma_lib::cpp::map::emplace, 2);
    rb_define_method(rb_cMap, "erase", kuma_lib::cpp::map::erase, 1);
    rb_define_method(rb_cMap, "merge!", kuma_lib::cpp::map::merge, 1);
    rb_define_method(rb_cMap, "[]", kuma_lib::cpp::map::bracket, 1);
    rb_define_method(rb_cMap, "at", kuma_lib::cpp::map::at, 1);
    rb_define_method(rb_cMap, "find", kuma_lib::cpp::map::find, 1);
    rb_define_method(rb_cMap, "include?", kuma_lib::cpp::map::include, 1);
    rb_define_alias(rb_cMap, "member?", "include?");
    rb_define_method(rb_cMap, "lower_bound", kuma_lib::cpp::map::lower_bound, 1);
    rb_define_alias(rb_cMap, ">=", "lower_bound");
    rb_define_method(rb_cMap, "not_lower_bound", kuma_lib::cpp::map::not_lower_bound, 1);
    rb_define_alias(rb_cMap, "<", "not_lower_bound");
    rb_define_method(rb_cMap, "upper_bound", kuma_lib::cpp::map::upper_bound, 1);
    rb_define_alias(rb_cMap, ">", "upper_bound");
    rb_define_method(rb_cMap, "not_upper_bound", kuma_lib::cpp::map::not_upper_bound, 1);
    rb_define_alias(rb_cMap, "<=", "not_upper_bound");
}