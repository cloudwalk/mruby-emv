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
mrb_emv_s_open(mrb_state *mrb, mrb_value klass)
{
  mrb_value com;
  mrb_get_args(mrb, "S", &com);

  return mrb_fixnum_value(PP_Open(RSTRING_PTR(com)));
}

static mrb_value
mrb_emv_s_close(mrb_state *mrb, mrb_value klass)
{
  mrb_value msg;
  mrb_get_args(mrb, "S", &msg);

  return mrb_fixnum_value(PP_Close(RSTRING_PTR(msg)));
}

static mrb_value
mrb_emv_s_abort(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(PP_Abort());
}

static mrb_value
mrb_emv_s_start_get_card(mrb_state *mrb, mrb_value klass)
{
  mrb_value value;
  mrb_get_args(mrb, "S", &value);

  return mrb_fixnum_value(PP_StartGetCard(RSTRING_PTR(value)));
}

static mrb_value
mrb_emv_s_get_card(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output, msg;
  mrb_value array;
  mrb_int ret;

  ret = PP_GetCard(output, msg);

  array  = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK)
    mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));
  else
    mrb_ary_push(mrb, array, mrb_nil_value());
  if (ret == PPCOMP_NOTIFY) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, msg));

  return array;
}

static mrb_value
mrb_emv_s_start_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  mrb_value process, tags, optional_tags;

  mrb_get_args(mrb, "SSS", &process, &tags, &optional_tags);

  return mrb_fixnum_value(PP_StartGoOnChip(RSTRING_PTR(process),
        RSTRING_PTR(tags), RSTRING_PTR(optional_tags)));
}

static mrb_value
mrb_emv_s_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output, msg;
  mrb_value array;
  mrb_int ret;

  ret = PP_GoOnChip(output, msg);

  array  = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK)
    mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));
  else
    mrb_ary_push(mrb, array, mrb_nil_value());
  if (ret == PPCOMP_NOTIFY) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, msg));

  return array;
}

static mrb_value
mrb_emv_s_finish_chip(mrb_state *mrb, mrb_value klass)
{
  mrb_value array, finish, tags;
  OUTPUT output;
  mrb_int ret;

  mrb_get_args(mrb, "SS", &finish, &tags);

  ret = PP_FinishChip(RSTRING_PTR(finish), RSTRING_PTR(tags), output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s_start_remove_card(mrb_state *mrb, mrb_value klass)
{
  mrb_value input;
  mrb_get_args(mrb, "S", &input);

  return mrb_fixnum_value(PP_StartRemoveCard(RSTRING_PTR(input)));
}

static mrb_value
mrb_pinpad_s_remove_card(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;
  mrb_value array;

  ret = PP_RemoveCard(output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

void
mrb_emv_init(mrb_state* mrb)
{
  struct RClass *plt, *emv;

  plt = mrb_class_get(mrb, "Platform");
  emv = mrb_define_class_under(mrb, plt, "EMV",  mrb->object_class);

  /*Control*/
  mrb_define_class_method(mrb , emv , "open"  , mrb_emv_s_open  , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "close" , mrb_emv_s_close , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "abort" , mrb_emv_s_abort , MRB_ARGS_NONE());

  /*EMV manipulation*/
  mrb_define_class_method(mrb , emv , "start_get_card"    , mrb_emv_s_start_get_card   , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "get_card"          , mrb_emv_s_get_card         , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "start_go_on_chip"  , mrb_emv_s_start_go_on_chip , MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb , emv , "go_on_chip"        , mrb_emv_s_go_on_chip       , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "finish_chip"       , mrb_emv_s_finish_chip      , MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb , emv , "start_remove_card" , mrb_pinpad_s_start_remove_card     , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "remove_card"       , mrb_pinpad_s_remove_card           , MRB_ARGS_NONE());

  /*
   *Debugs
   *void PPEXP PP_SetDebugFunc (void (CALLBACK * pfDebugFunc) (char cData));
   *void PPEXP PP_SetDisplayFunc (void (CALLBACK * pfDispFunc)(int iRow, int iCol, PPCOMP_COMMANDS eCommand, char cReverse, char * pszMsg));
   *int PPEXP PP_SendPropCmd (INPUT psCmd, INPUT pszInput, OUTPUT pszOutput);
   *int PPEXP PP_OpenHandle (void *pComHandle);
   *int PPEXP PP_OpenFast (INPUT psCom);
   */

  /*
   *int PPEXP PP_GetTimeStamp (INPUT psInput, OUTPUT psOutput);
   *TODO Scalone: Check implementation is necessary
   *mrb_define_class_method(mrb , emv , "resume_get_card"  , mrb_emv_s_resume_get_card  , MRB_ARGS_NONE());
   */
}

