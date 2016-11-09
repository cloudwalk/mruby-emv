#include <stdlib.h>
#include <stdio.h>

#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/hash.h"
#include "ppcomp.h"

/*PP_InitLib();*/
/*PP_SetDisplayFunc(&Display);*/
/*PP_Open("01");*/
/*ppRet = PP_GetInfo("00", psOutput);*/
/*PP_TableLoadInit ("03" TIMESTAMP);*/
/*PP_TableLoadRec ("01" "2841030507A000000003101000000000000000000001CREDITO         030084008300000769862020000080750001544112345678E0F0D87000F0F00122D84000A8000010000000D84004F800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000Y1Z1Y3Z3");*/
/*PP_TableLoadEnd ();*/
/*PP_StartGetCard("03" "99" "000000000000" "150605123456" TIMESTAMP "00");*/
/*ppRet  = PP_GetCard (psOutput, psMsgNotify);*/
/*PP_StartGetPIN("30100000000000000000000000000000000161234567890123456   10412DIGITE SUA SENHA                ");*/
/*ppRet  = PP_GetPIN(psOutput, psMsgNotify);*/
/*PP_StartRemoveCard("    REMOVA        O CARTÃO     ");*/
/*ppRet = PP_RemoveCard(psMsgNotify);*/
/*PP_Close("");*/

/*static mrb_value*/
/*mrb_emv_s_init(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*PP_InitLib();*/
  /*return mrb_nil_value();*/
/*}*/

/*static mrb_value*/
/*mrb_emv_s_version(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*OUTPUT version;*/

  /*PP_GetLibVersion(version);*/

  /*return mrb_str_new_cstr(mrb, version);*/
/*}*/

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

static mrb_value
mrb_emv_s_start_get_card(mrb_state *mrb, mrb_value klass)
{
  mrb_value value;
  value = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@init"));
  return mrb_fixnum_value(PP_StartGetCard(RSTRING_PTR(value)));
}

static mrb_value
mrb_emv_s_get_card(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output, msg;

  ret = PP_GetCard(output, msg);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@init_info"),
      mrb_str_new_cstr(mrb, output));
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@init_message_notify"),
      mrb_str_new_cstr(mrb, msg));

  return mrb_fixnum_value(ret);
}

/*static mrb_value*/
/*mrb_emv_s_resume_get_card(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*return mrb_fixnum_value(PP_ResumeGetCard());*/
/*}*/

static mrb_value
mrb_emv_s_start_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  mrb_value process, tags, optional_tags;

  process       = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@process"));
  tags          = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@tags"));
  optional_tags = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@optional_tags"));

  return mrb_fixnum_value(PP_StartGoOnChip(RSTRING_PTR(process),
        RSTRING_PTR(tags), RSTRING_PTR(optional_tags)));
}

static mrb_value
mrb_emv_s_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output, msg;
  mrb_int ret;

  ret = PP_GoOnChip(output, msg);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@process_info"),
      mrb_str_new_cstr(mrb, output));
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@process_message_notify"),
      mrb_str_new_cstr(mrb, msg));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_emv_s_finish_chip(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT result;
  mrb_value finish, tags;

  finish = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@finish"));
  tags   = mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "@finish_tags"));

  ret = PP_FinishChip(RSTRING_PTR(finish), RSTRING_PTR(tags), result);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@finish_info"),
      mrb_str_new_cstr(mrb, result));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_get_info(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  ret = PP_GetInfo(RSTRING_PTR(input), output);
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@info"),
      mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_working_key_m(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  ret = PP_DefineWKPAN(RSTRING_PTR(input), output);
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@working_key"),
      mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_display(mrb_state *mrb, mrb_value klass)
{
  mrb_value msg;
  mrb_get_args(mrb, "S", &msg);

  return mrb_fixnum_value(PP_Display(RSTRING_PTR(msg)));
}

/*static mrb_value*/
/*mrb_pinpad_s_display_ex(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_value msg;*/
  /*mrb_get_args(mrb, "S", &msg);*/

  /*return mrb_fixnum_value(PP_DisplayEx(RSTRING_PTR(msg)));*/
/*}*/

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
  OUTPUT output, msg;

  ret = PP_GetPIN(output, msg);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@pin_info"),
      mrb_str_new_cstr(mrb, output));
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@pin_message_notify"),
      mrb_str_new_cstr(mrb, msg));

  return mrb_fixnum_value(ret);
}

/*static mrb_value*/
/*mrb_pinpad_s_start_check_event(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_int ret;*/
  /*mrb_value input;*/
  /*mrb_get_args(mrb, "S", &input);*/

  /*ret = PP_StartCheckEvent(RSTRING_PTR(input));*/

  /*return mrb_fixnum_value(ret);*/
/*}*/

/*static mrb_value*/
/*mrb_pinpad_s_check_event(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_int ret;*/
  /*OUTPUT output;*/

  /*ret = PP_CheckEvent(output);*/

  /*mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@event_info"),*/
      /*mrb_str_new_cstr(mrb, output));*/

  /*return mrb_fixnum_value(ret);*/
/*}*/

static mrb_value
mrb_pinpad_s_encrypt_buffer(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  ret = PP_EncryptBuffer(RSTRING_PTR(input), output);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@encryption_info"),
      mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_pinpad_s_get_dukpt(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;
  mrb_value input;

  mrb_get_args(mrb, "S", &input);

  ret = PP_GetDUKPT(RSTRING_PTR(input), output);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@dukpt_info"),
      mrb_str_new_cstr(mrb, output));

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
  OUTPUT output;

  ret = PP_ChipDirect(output);

  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@chip_info"),
      mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

/*static mrb_value*/
/*mrb_pinpad_s_start_remove_card(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_value input;*/
  /*mrb_get_args(mrb, "S", &input);*/

  /*return mrb_fixnum_value(PP_StartRemoveCard(RSTRING_PTR(input)));*/
/*}*/

static mrb_value
mrb_pinpad_s_remove_card(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT output;

  ret = PP_RemoveCard(output);
  mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@remove_info"),
      mrb_str_new_cstr(mrb, output));

  return mrb_fixnum_value(ret);
}

/*static mrb_value*/
/*mrb_pinpad_s_start_generic_command(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_value input;*/
  /*mrb_get_args(mrb, "S", &input);*/

  /*return mrb_fixnum_value(PP_StartGenericCmd(RSTRING_PTR(input)));*/
/*}*/

/*static mrb_value*/
/*mrb_pinpad_s_generic_command(mrb_state *mrb, mrb_value klass)*/
/*{*/
  /*mrb_int ret;*/
  /*OUTPUT output, msg;*/

  /*ret = PP_GenericCmd(output, msg);*/

  /*mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@cmd_info"),*/
      /*mrb_str_new_cstr(mrb, output));*/
  /*mrb_cv_set(mrb, klass, mrb_intern_lit(mrb, "@cmd_message_notify"),*/
      /*mrb_str_new_cstr(mrb, msg));*/

  /*return mrb_fixnum_value(ret);*/
/*}*/

void
mrb_emv_init(mrb_state* mrb)
{
  struct RClass *plt, *emv, *table, *pinpad;

  plt    = mrb_class_get(mrb, "Platform");
  emv    = mrb_define_class_under(mrb, plt, "EMV",  mrb->object_class);
  table  = mrb_define_class_under(mrb, emv, "Table",  mrb->object_class);
  pinpad = mrb_define_class_under(mrb, emv, "Pinpad",  mrb->object_class);

  /*Required*/
  /*mrb_define_class_method(mrb , emv , "init"    , mrb_emv_s_init    , MRB_ARGS_NONE());*/
  /*mrb_define_class_method(mrb , emv , "version" , mrb_emv_s_version , MRB_ARGS_NONE());*/

  /*void PPEXP PP_SetDebugFunc (void (CALLBACK * pfDebugFunc) (char cData));*/
  /*void PPEXP PP_SetDisplayFunc (void (CALLBACK * pfDispFunc)(int iRow, int iCol, PPCOMP_COMMANDS eCommand, char cReverse, char * pszMsg));*/
  /*int PPEXP PP_SendPropCmd (INPUT psCmd, INPUT pszInput, OUTPUT pszOutput);*/
  /*int PPEXP PP_OpenHandle (void *pComHandle);*/
  /*int PPEXP PP_OpenFast (INPUT psCom);*/

  /*Control*/
  mrb_define_class_method(mrb , emv , "open"  , mrb_emv_s_open  , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "close" , mrb_emv_s_close , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "abort" , mrb_emv_s_abort , MRB_ARGS_NONE());

  /*Table*/
  mrb_define_class_method(mrb , table , "init"   , mrb_emv_table_s_init   , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , table , "rec"    , mrb_emv_table_s_rec    , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , table , "fin"    , mrb_emv_table_s_fin    , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , table , "change" , mrb_emv_table_s_change , MRB_ARGS_REQ(1));

  /*TODO Scalone to define*/
  /*int PPEXP PP_GetTimeStamp (INPUT psInput, OUTPUT psOutput);*/

  /*SmartCard Process*/
  mrb_define_class_method(mrb , emv , "start_get_card"   , mrb_emv_s_start_get_card   , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "get_card"         , mrb_emv_s_get_card         , MRB_ARGS_NONE());
  /*mrb_define_class_method(mrb , emv , "resume_get_card"  , mrb_emv_s_resume_get_card  , MRB_ARGS_NONE());*/
  mrb_define_class_method(mrb , emv , "start_go_on_chip" , mrb_emv_s_start_go_on_chip , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "go_on_chip"       , mrb_emv_s_go_on_chip       , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "finish_chip"      , mrb_emv_s_finish_chip      , MRB_ARGS_NONE());

  /*Pinpad*/
  mrb_define_class_method(mrb , pinpad , "get_info"              , mrb_pinpad_s_get_info              , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "working_key="          , mrb_pinpad_s_working_key_m         , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "display"               , mrb_pinpad_s_display               , MRB_ARGS_REQ(1));
  /*mrb_define_class_method(mrb , pinpad , "display_ex"            , mrb_pinpad_s_display_ex            , MRB_ARGS_REQ(1));*/
  mrb_define_class_method(mrb , pinpad , "start_get_key"         , mrb_pinpad_s_start_get_key         , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "get_key"               , mrb_pinpad_s_get_key               , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , pinpad , "start_get_pin"         , mrb_pinpad_s_start_get_pin         , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "get_pin"               , mrb_pinpad_s_get_pin               , MRB_ARGS_NONE());
  /*mrb_define_class_method(mrb , pinpad , "start_check_event"     , mrb_pinpad_s_start_check_event     , MRB_ARGS_REQ(1));*/
  /*mrb_define_class_method(mrb , pinpad , "check_event"           , mrb_pinpad_s_check_event           , MRB_ARGS_REQ(1));*/
  mrb_define_class_method(mrb , pinpad , "encrypt_buffer"        , mrb_pinpad_s_encrypt_buffer        , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "get_dukpt"             , mrb_pinpad_s_get_dukpt             , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "start_chip_direct"     , mrb_pinpad_s_start_chip_direct     , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , pinpad , "chip_direct"           , mrb_pinpad_s_chip_direct           , MRB_ARGS_NONE());
  /*mrb_define_class_method(mrb , pinpad , "start_remove_card"     , mrb_pinpad_s_start_remove_card     , MRB_ARGS_REQ(1));*/
  mrb_define_class_method(mrb , pinpad , "remove_card"           , mrb_pinpad_s_remove_card           , MRB_ARGS_NONE());
  /*mrb_define_class_method(mrb , pinpad , "start_generic_command" , mrb_pinpad_s_start_generic_command , MRB_ARGS_REQ(1));*/
  /*mrb_define_class_method(mrb , pinpad , "generic_command"       , mrb_pinpad_s_generic_command       , MRB_ARGS_NONE());*/
  /*mrb_define_class_method(mrb , pinpad , "check_event_v108"      , mrb_pinpad_s_check_event_v108      , MRB_ARGS_REQ(1));*/
  /*int PPEXP PP_StartCheckEventV108 (INPUT psInput);*/
}

