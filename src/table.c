#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/hash.h"
#include "ppcomp.h"

static mrb_value
mrb_emv_table_s_init(mrb_state *mrb, mrb_value klass)
{
  mrb_value acquirer;
  mrb_get_args(mrb, "S", &acquirer);

  return mrb_fixnum_value(PP_TableLoadInit(RSTRING_PTR(acquirer)));
}

static mrb_value
mrb_emv_table_s_rec(mrb_state *mrb, mrb_value klass)
{
  mrb_value table;
  mrb_get_args(mrb, "S", &table);

  return mrb_fixnum_value(PP_TableLoadRec(RSTRING_PTR(table)));
}

static mrb_value
mrb_emv_table_s_fin(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(PP_TableLoadEnd());
}

static mrb_value
mrb_emv_table_s_change(mrb_state *mrb, mrb_value klass)
{
  mrb_value table;
  mrb_get_args(mrb, "S", &table);

  return mrb_fixnum_value(PP_ChangeParameter(RSTRING_PTR(table)));
}

void
mrb_table_init(mrb_state* mrb)
{
  struct RClass *plt, *emv, *table;

  plt   = mrb_class_get(mrb, "Platform");
  emv   = mrb_class_get_under(mrb, plt, "EMV");
  table = mrb_define_class_under(mrb, emv, "Table", mrb->object_class);

  mrb_define_class_method(mrb , table , "init"   , mrb_emv_table_s_init   , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , table , "rec"    , mrb_emv_table_s_rec    , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , table , "fin"    , mrb_emv_table_s_fin    , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , table , "change" , mrb_emv_table_s_change , MRB_ARGS_REQ(1));
}

