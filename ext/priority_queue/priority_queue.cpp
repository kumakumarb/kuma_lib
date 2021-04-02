#include <ruby.h>

#include <queue>
#include <functional>

using ll = long long int;
using data_t = std::pair<void *, ll>;
using fn_t = std::function<bool(const data_t&, const data_t&)>;
using pq_t = std::priority_queue<data_t, std::vector<data_t>, fn_t>;

struct PrioQueue : public pq_t {
  private:
    VALUE _block;
    inline ll eval(VALUE val) const {return NUM2LL(rb_funcall(_block, rb_intern("call"), 1, val));}

  public:
    PrioQueue(fn_t f, VALUE block) : pq_t(f), _block(block) {}

    void mark(void) const {
        rb_gc_mark(_block);
        for (const data_t& e : c) rb_gc_mark(VALUE(e.first));
    }

    void push(VALUE val) {
        pq_t::push({(void *)val, eval(val)});
    }

    void clear() {
        c.clear();
    }
};

VALUE rb_mKumaLib, rb_cPrioQueue;

fn_t cmp = [](const data_t& d1, const data_t& d2) {
    return d1.second < d2.second;
};

struct WrapPrioQueue {
    PrioQueue *pq;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static void mark_prio_queue(void *ptr) {
    ((WrapPrioQueue *)ptr)->pq->mark();
}

static void free_prio_queue(void *ptr) {
    WrapPrioQueue *wpq = (WrapPrioQueue *)ptr;
    if (wpq->pq) delete wpq->pq;
    xfree(ptr);
}

static size_t memsize_prio_que(const void *ptr) {
    return ((WrapPrioQueue *)ptr)->pq->size() * sizeof(data_t);
}

static const rb_data_type_t prio_queue_type {
    "PriorityQueue",
    {mark_prio_queue, free_prio_queue, memsize_prio_que,},
    0, 0,
    0,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define GetWrapPrioQueue(obj, wpq) \
    WrapPrioQueue *(wpq); \
    TypedData_Get_Struct((obj), WrapPrioQueue, &prio_queue_type, (wpq));

static VALUE alloc_prio_queue(VALUE klass) {
    WrapPrioQueue *wpq = nullptr;
    return TypedData_Make_Struct(klass, WrapPrioQueue, &prio_queue_type, wpq);
}

static VALUE init_prio_queue(VALUE self) {
    VALUE block = rb_block_proc();
    GetWrapPrioQueue(self, wpq)
    wpq->pq = new PrioQueue(cmp, block);
    return Qnil;
}

static VALUE empty_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    return wpq->pq->empty() ? Qtrue : Qfalse;
}

static VALUE size_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    return LL2NUM(wpq->pq->size());
}

static VALUE top_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    return wpq->pq->empty() ? Qnil : VALUE(wpq->pq->top().first);
}

static VALUE push_prio_queue(VALUE self, VALUE val) {
    GetWrapPrioQueue(self, wpq)
    wpq->pq->push(val);
    return self;
}

static VALUE push_some_prio_que(int argc, VALUE *argv, VALUE self) {
    GetWrapPrioQueue(self, wpq);
    for (int i = 0; i < argc; i++) wpq->pq->push(argv[i]);
    return Qnil;
}

static VALUE pop_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    wpq->pq->pop();
    return Qnil;
}

static VALUE shift_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    if (wpq->pq->empty()) return Qnil;
    VALUE r = VALUE(wpq->pq->top().first);
    wpq->pq->pop();
    return r;
}

static VALUE clear_prio_queue(VALUE self) {
    GetWrapPrioQueue(self, wpq)
    wpq->pq->clear();
    return Qnil;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void Init_priority_queue(void) {
    rb_mKumaLib = rb_define_module("KumaLib");
    rb_cPrioQueue = rb_define_class_under(rb_mKumaLib, "PriorityQueue", rb_cObject);

    rb_undef_alloc_func(rb_cPrioQueue);
    rb_define_alloc_func(rb_cPrioQueue, alloc_prio_queue);

    rb_define_method(rb_cPrioQueue, "initialize", init_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "empty?", empty_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "size", size_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "top", top_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "<<", push_prio_queue, 1);
    rb_define_method(rb_cPrioQueue, "push", push_some_prio_que, -1);
    rb_define_method(rb_cPrioQueue, "pop", pop_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "shift", shift_prio_queue, 0);
    rb_define_method(rb_cPrioQueue, "clear", clear_prio_queue, 0);
}