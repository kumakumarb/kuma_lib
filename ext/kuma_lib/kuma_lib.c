#include "kuma_lib.h"

VALUE rb_mKumaLib;

static VALUE print() {printf("OK!\n"); return Qnil;}

void
Init_kuma_lib(void)
{
  rb_mKumaLib = rb_define_module("KumaLib");
  VALUE xxx = rb_define_class_under(rb_mKumaLib, "Hoge", rb_cObject);
  rb_define_singleton_method(xxx, "print", print, 0);
}
