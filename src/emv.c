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

/*----------------------*/
/* Flags para DSP_Text  */
/*----------------------*/

/* Comportamento básico da função DSP_Text */

#define DSP_F_BHAVMASK   0x00000F00
#define DSP_F_FREETXT    0x00000000 // Texto "livre" em pszTxt1, linhas separadas por '\r'. pszTxt2 não é usado.
#define DSP_F_TXT2X16    0x00000100 // Texto de tamanho fixo 2x16 (32 caracteres sem separação) em pszTxt1. pszTxt2 não é usado.
#define DSP_F_DATAENTRY  0x00000200 // Título em pszTxt1 (pode ter mais de uma linha separada por '\r'). pszTxt2 é dado sendo editado.

/* Imagens / ícones a serem apresentados juntamente com o texto */

#define DSP_F_ICONMASK   0x000000FF
#define DSP_F_ICORMC     0x00000001 // Remoção de cartão
#define DSP_F_ICOPCRD    0x00000002 // Apresentação de cartão
#define DSP_F_ICOCTLS    0x00000003 // Apresentação de cartão contactless
#define DSP_F_ICOERROR   0x00000004 // Erro ou "warning"
#define DSP_F_ICOSTANIM  0x00000081 // Inicia animação (de passagem de tempo)
#define DSP_F_ICOANIM    0x00000082 // Continua animação (de passagem de tempo)
#define DSP_F_ICOLED     0x00000040 // 0x0000004x, onde x entre 0 e 4 led na tela, e F apaga Leds.

/* Textos fixos, para uso se aplicação desejar alterar a original */
#define DSP_F_TXTNMASK        0x000F0000
#define DSP_F_TXTPROCESSING   0x00010000  // "PROCESSANDO"
#define DSP_F_TXTINSSC        0x00020000  // "INSIRA OU PASSE O CARTAO"
#define DSP_F_TXTINSPSC       0x00030000  // "APROXIME, INSIRA OU PASSE CARTAO"
#define DSP_F_TXTSEL          0x00040000  // "SELECIONE"
#define DSP_F_TXTAPPINV       0x00050000  // "APLICACAO INVALIDA"
#define DSP_F_TXTINVPASS      0x00060000  // "SENHA INVALIDA"
#define DSP_F_TXTBLKCARD      0x00070000  // "CARTAO BLOQUEADO"
#define DSP_F_TXTBLKPASS      0x00080000  // "SENHA BLOQUEADA "
#define DSP_F_TXTNEXTBLK      0x00090000  // "PROXIMO ERRO BLOQUEIA SENHA "
#define DSP_F_TXTOKPASS       0x000A0000  // "SENHA VERIFICADA"
#define DSP_F_TXTREMOVCARD    0x000B0000  // "RETIRE O CARTAO "
#define DSP_F_TXTLTABLE       0x000C0000  // "ATUALIZANDO TABELAS"

/* Alinhamento do texto */

#define DSP_F_ALIGNMASK  0x0000F000
#define DSP_F_LEFT       0x00000000
#define DSP_F_CENTER     0x00001000
#define DSP_F_RIGHT      0x00002000

/*--------------------------*/
/* Flags para DSP_iMenuShow */
/*--------------------------*/

#define DSPMNU_UPARROW     0x00000001  // Mostra "seta para cima"
#define DSPMNU_DOWNARROW   0x00000002  // Mostra "seta para baixo"

#define DSPMNU_KEYTOUCH    0x80000000  // Touch executado, 0x80XXXYYY, onde XXX é a posição em X, e YYY a posição em Y

#define DSP_F_HOTKEY       0x00800000  // Informa se o menu vai suportar hotkey.
#define DSP_F_BLOCK        0x00400000  // Informa se o menu será "blocante na função DSP_iMenuShow

#define FONT_H	12
#define FONT_W	8

mrb_state *current_mrb;
mrb_value current_klass;

/*static char szTitle[32];*/

void GDSP_Clear (void)
{
  GEDI_LCD_Clear();
  /*mrb_value display;*/
  /*struct RClass *device;*/

  /*device  = mrb_class_get(current_mrb, "Device");*/
  /*display = mrb_const_get(current_mrb, mrb_obj_value(device), mrb_intern_lit(mrb, "Display"));*/

  /*mrb_funcall(current_mrb, display, "clear", 0);*/
}

int GDSP_iMenuStart (const char *pszTitle, unsigned long *pulFlags)
{
  mrb_value ret;
  /* Initialize */
  strncpy(szTitle, pszTitle, 32);

  /* Don't use hotkey */
  *pulFlags = 0;

  /*ret = mrb_funcall(current_mrb, current_klass, "internal_set_menu", 1,*/
      /*mrb_str_new(current_mrb, pszTitle, 32));*/

  /*return mrb_fixnum(ret);*/
  return 4;
}



/**
 * See libdisplay.h
 */
void GDSP_Text (unsigned long ulFlags, const char *pszTxt1, const char *pszTxt2)
{
  unsigned int w, h;
  char* p;
  char value[64];
  int i = 1;

  memset(value, 0, sizeof(value));
  if(pszTxt1)
  {
    strcpy(value, pszTxt1);
    GEDI_LCD_ResolutionGet(&w, &h);
    GEDI_LCD_DrawRectangle(0, FONT_H * 0, w, FONT_H * 4, FALSE);

    p = strtok(value, "\r\n");
    while(p != NULL)
    {
      /* Quebra de linha */
      do
      {
        GEDI_LCD_DrawString(0, FONT_H * i++, FONT_W, FONT_H, "%.16s", p);
        p += (strlen(p) >= 16) ? 16 : strlen(p);
      } while(*p);

      p = strtok(NULL, "\r\n");
    }
  }

  /* Txt 2 */
  if(ulFlags & DSP_F_DATAENTRY)
  {
    printf("[DSP_Text] Data entry: %s\n", pszTxt2);
    strcpy(value, pszTxt2);

    p = strtok(value, "\r\n");
    while(p != NULL)
    {
      /* Quebra de linha */
      do
      {
        GEDI_LCD_DrawString(0, FONT_H * i++, FONT_W, FONT_H, "%.16s", p);
        p += (strlen(p) >= 16) ? 16 : strlen(p);
      } while(*p);
      p = strtok(NULL, "\r\n");
    }
  }

}

/**
 *
 */
int GDSP_iMenuShow (unsigned long ulFlags, const char *pszOpts, int iOptSel)
{
  int i = 0;
  char value[32];
  char* p;

  /* Titulo */
  GEDI_LCD_DrawString(0, FONT_H * i++, FONT_W, FONT_H, szTitle);

  /* Copia valores para o menu */
  strcpy(value, pszOpts);
  p = strtok(value, "\r");
  while(p != NULL)
  {
    if(iOptSel == (i - 1)) GEDI_LCD_DrawString(0, FONT_H * i++, FONT_W, FONT_H, "> %s", p);
    else                   GEDI_LCD_DrawString(0, FONT_H * i++, FONT_W, FONT_H, "  %s", p);
    p = strtok(NULL, "\r");
  }

  return 0;
}

  static mrb_value
mrb_emv_s_open(mrb_state *mrb, mrb_value klass)
{
  mrb_value com;
  DSP_Callback_Stru dsp_st;

  mrb_get_args(mrb, "S", &com);

  PP_InitLib();

  dsp_st.pClear = &GDSP_Clear;
  dsp_st.pText = &GDSP_Text;
  dsp_st.piMenuShow = &GDSP_iMenuShow;
  dsp_st.piMenuStart = &GDSP_iMenuStart;
  /*Callbacks*/
  PP_DspCallbacks(&dsp_st);

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
  OUTPUT out[11]={0x00};
  mrb_int ret;

  mrb_get_args(mrb, "S", &value);

  return mrb_fixnum_value(PP_StartGetCard(RSTRING_PTR(value)));
}

  static mrb_value
mrb_emv_s_get_card(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
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
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
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
  OUTPUT output[1024]={0x00};
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
  OUTPUT output[1024]={0x00};
  mrb_value array;

  ret = PP_RemoveCard(output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));

  return array;
}

  static mrb_value
mrb_emv_s_timestamp(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  OUTPUT out[11];
  mrb_value acquirer, array;
  mrb_get_args(mrb, "S", &acquirer);

  ret = PP_GetTimeStamp(RSTRING_PTR(acquirer), out);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));

  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new(mrb, out, 10));

  return array;
}

  void
mrb_emv_init(mrb_state* mrb)
{
  struct RClass *plt, *emv;

  plt = mrb_class_get(mrb, "Platform");
  emv = mrb_define_class_under(mrb, plt, "EMV",  mrb->object_class);

  /*Control*/
  mrb_define_class_method(mrb , emv , "open"      , mrb_emv_s_open      , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "close"     , mrb_emv_s_close     , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "abort"     , mrb_emv_s_abort     , MRB_ARGS_NONE());

  /*EMV manipulation*/
  mrb_define_class_method(mrb , emv , "start_get_card"    , mrb_emv_s_start_get_card       , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "get_card"          , mrb_emv_s_get_card             , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "start_go_on_chip"  , mrb_emv_s_start_go_on_chip     , MRB_ARGS_REQ(3));
  mrb_define_class_method(mrb , emv , "go_on_chip"        , mrb_emv_s_go_on_chip           , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "finish_chip"       , mrb_emv_s_finish_chip          , MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb , emv , "start_remove_card" , mrb_pinpad_s_start_remove_card , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb , emv , "remove_card"       , mrb_pinpad_s_remove_card       , MRB_ARGS_NONE());
  mrb_define_class_method(mrb , emv , "timestamp"         , mrb_emv_s_timestamp            , MRB_ARGS_REQ(1));

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

