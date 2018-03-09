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
mrb_pinpad_s_info(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value input, array;

  mrb_get_args(mrb, "S", &input);

  ret = PP_GetInfo(RSTRING_PTR(input), output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s_set_working_key(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value input, array;

  mrb_get_args(mrb, "S", &input);

  ret = PP_DefineWKPAN(RSTRING_PTR(input), output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s_display(mrb_state *mrb, mrb_value klass)
{
  mrb_value msg;
  mrb_get_args(mrb, "S", &msg);

  return mrb_fixnum_value(PP_Display(RSTRING_PTR(msg)));
}

static mrb_value
mrb_pinpad_s_start_get_key(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(PP_StartGetKey());
}

static mrb_value
mrb_pinpad_s_get_key(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(PP_GetKey());
}

static mrb_value
mrb_pinpad_s_start_get_pin(mrb_state *mrb, mrb_value klass)
{
  mrb_value input;
  mrb_get_args(mrb, "S", &input);

  return mrb_fixnum_value(PP_StartGetPIN(RSTRING_PTR(input)));
}

static mrb_value
mrb_pinpad_s_get_pin(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
  mrb_value array;

  ret = PP_GetPIN(output, msg);

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
mrb_pinpad_s_start_check_event(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  ret = PP_StartCheckEvent(RSTRING_PTR(input));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_check_event(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value array;

  ret = PP_CheckEvent(output);

  array  = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s__encrypt_buffer(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value input, array;

  mrb_get_args(mrb, "S", &input);

  ret = PP_EncryptBuffer(RSTRING_PTR(input), output);

  array  = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s_get_dukpt(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value input, array;

  mrb_get_args(mrb, "S", &input);

  ret   = PP_GetDUKPT(RSTRING_PTR(input), output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_start_chip_direct(mrb_state *mrb, mrb_value klass)
{
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  return mrb_fixnum_value(PP_StartChipDirect(RSTRING_PTR(input)));
}

static mrb_value
mrb_pinpad_s_chip_direct(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00};
  mrb_value array;

  ret = PP_ChipDirect(output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

static mrb_value
mrb_pinpad_s_start_generic_command(mrb_state *mrb, mrb_value klass)
{
  mrb_value input;
  mrb_get_args(mrb, "S", &input);

  return mrb_fixnum_value(PP_StartGenericCmd(RSTRING_PTR(input)));
}

static mrb_value
mrb_pinpad_s_generic_command(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
  mrb_value array;

  ret = PP_GenericCmd(output, msg);

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
mrb_pinpad_s_key_test(mrb_state *mrb, mrb_value klass)
{
  mrb_int type, operation, index, ret;

  mrb_get_args(mrb, "iii", &type, &operation, &index);

  return mrb_fixnum_value(GEDI_KMS_KeyPresenceTest(type, operation, index));
}

static mrb_value
mrb_pinpad_s__key_kcv(mrb_state *mrb, mrb_value klass)
{
  mrb_value array;
  unsigned char kcv[3] = {0x00};
  mrb_int type, operation, index, ret;

  mrb_get_args(mrb, "iii", &type, &operation, &index);

  ret = GEDI_KMS_KCVGet(type, operation, index, &kcv);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new(mrb, kcv, 4));

  return array;
}

static mrb_value
mrb_pinpad_s__key_ksn(mrb_state *mrb, mrb_value klass)
{
  mrb_value array;
  unsigned char ksn[10] = {0x00};
  mrb_int type, operation, index, ret;

  mrb_get_args(mrb, "iii", &type, &operation, &index);

  ret = GEDI_KMS_DUKPTKSNGet(type, operation, index, &ksn);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new(mrb, ksn, 10));

  return array;
}

void
mrb_pinpad_init(mrb_state* mrb)
{
  struct RClass *plt, *emv, *pinpad;

  plt    = mrb_class_get(mrb, "Platform");
  emv    = mrb_class_get_under(mrb, plt, "EMV");
  pinpad = mrb_define_class_under(mrb, emv, "Pinpad", mrb->object_class);

  /*Pinpad*/
  mrb_define_class_method(mrb , pinpad , "info"                  , mrb_pinpad_s_info                  , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "set_working_key"       , mrb_pinpad_s_set_working_key       , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "display"               , mrb_pinpad_s_display               , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "start_get_key"         , mrb_pinpad_s_start_get_key         , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "get_key"               , mrb_pinpad_s_get_key               , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "start_get_pin"         , mrb_pinpad_s_start_get_pin         , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "get_pin"               , mrb_pinpad_s_get_pin               , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "start_check_event"     , mrb_pinpad_s_start_check_event     , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "check_event"           , mrb_pinpad_s_check_event           , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "_encrypt_buffer"       , mrb_pinpad_s__encrypt_buffer       , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "get_dukpt"             , mrb_pinpad_s_get_dukpt             , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "start_chip_direct"     , mrb_pinpad_s_start_chip_direct     , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "chip_direct"           , mrb_pinpad_s_chip_direct           , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "start_generic_command" , mrb_pinpad_s_start_generic_command , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "generic_command"       , mrb_pinpad_s_generic_command       , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "key_test"              , mrb_pinpad_s_key_test              , MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb , pinpad , "_key_kcv"              , mrb_pinpad_s__key_kcv               , MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb , pinpad , "_key_ksn"              , mrb_pinpad_s__key_ksn               , MRB_ARGS_REQ(3));

  /*
   *TODO Scalone: Check if is necessary implmentation.
   *mrb_define_class_method(mrb , pinpad , "display_ex"            , mrb_pinpad_s_display_ex            , MRB_ARGS_REQ(1));
   *mrb_define_class_method(mrb , pinpad , "check_event_v108"      , mrb_pinpad_s_check_event_v108      , MRB_ARGS_REQ(1));
   *int PPEXP PP_StartCheckEventV108 (INPUT psInput);
   */
}
