#include "kuma_lib.h"

VALUE rb_mKumaLib;

void
Init_kuma_lib(void)
{
  rb_mKumaLib = rb_define_module("KumaLib");
}
