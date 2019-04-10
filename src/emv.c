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

#ifdef __FRAMEWORK_TELIUM_PLUS__
#include "bc.h"
#include <sdk_tplus.h>
#include <OSL_Logger.h>
#include <pinpadEmv.h>
#include <larlib/log.h>
#include <abecs.h>
#include <pinpadLog.h>
#include <goalDisplay.h>
#define PPCOMP_OK PP_OK
#define PPCOMP_NOTIFY PP_NOTIFY
#else
#include "ppcomp.h"
#endif

#ifdef PAX
#include "ppcomp_ex.h"
#endif

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

#ifdef __FRAMEWORK_TELIUM_PLUS__
extern int getChar(int timeout);

int bcShowMenu (ppMessageType_t titleId, const char *titleText, const char *menu[], unsigned int nItems, unsigned int timeout) {
  int ch, i = 0, iRet = 1;
  mrb_value ret;
  char menu_string[100];
  char buf[50];

  memset(menu_string, 0, sizeof(menu_string));
  strncat(menu_string, "SELECIONE:\n", 11);
  for (i = 0; i < nItems; i++) {
    memset(buf, 0, sizeof(buf));
    sprintf(&buf[0], "%d. %s\n", i+1, menu[i]);
    strcat(menu_string, buf);
  }

  formatAndPrintDisplay(menu_string, 1, 0);

  do ch = getChar(60000);
  while (ch == GL_RESULT_TIMEOUT);

  if ('1' <= ch && ch <= '9')
    iRet = ch - '1';
  else
    iRet = BASE_ERR_CANCEL;

  // ret = mrb_funcall(current_mrb, current_klass, "internal_menu_show", 1, mrb_str_new_cstr(current_mrb, menu_string));
  //OSL_Warning("bcShowMenu:result = %d", iRet);

  return iRet;
}

static const char *getMessageStr(ppMessageType_t messageId) {
  switch (messageId) {
    case PPMSG_TEXT_S: return NULL;
    case PPMSG_PROCESSING: return "PROCESSANDO";
    case PPMSG_INSERT_SWIPE_CARD: return "INSIRA OU PASSE O CARTAO";
    case PPMSG_TAP_INSERT_SWIPE_CARD: return "APROXIME, INSIRA\nOU PASSE O CARTAO";
    case PPMSG_SELECT: return "SELECIONE";
    case PPMSG_SELECTED_S: return "SELECIONADO:\n%s";
    case PPMSG_INVALID_APP: return "APLICACAO INVALIDA";
    case PPMSG_WRONG_PIN_S: return "PIN ERRADO\n%s TENTATIVAS";
    case PPMSG_PIN_LAST_TRY: return "ULTIMA TENTATIVA";
    case PPMSG_PIN_BLOCKED: return "PIN BLOQUEADO";
    case PPMSG_PIN_VERIFIED: return NULL;
    case PPMSG_CARD_BLOCKED: return "CARTAO BLOQUEADO";
    case PPMSG_REMOVE_CARD: return NULL;
    case PPMSG_UPDATING_TABLES: return NULL;
    case PPMSG_UPDATING_RECORD: return NULL;
    case PPMSG_SECOND_TAP: return "RE-APROXIME O CARTAO";
    default: NULL;
  }
}

int bcShowMessage (ppMessageType_t messageId, const char *messageText) {
  char msg[256]={0x00};

  //OSL_Warning("bcShowMessage [%s] [%d] [%s]", getMessageStr(messageId), messageId, messageText);

  if (getMessageStr(messageId)) {
    goalClearScreen(TRUE);

    if (messageText)
      sprintf(msg, getMessageStr(messageId), messageText);
    else
      strcpy(msg, getMessageStr(messageId));
    formatAndPrintDisplay(msg, 2, 0);

    if (messageId == PPMSG_WRONG_PIN_S || messageId == PPMSG_PIN_LAST_TRY || messageId == PPMSG_PIN_BLOCKED || messageId == PPMSG_CARD_BLOCKED) sleep(3);
  }
  return 0;
}

void showPinImage(void) {
  long fileSize = 0;
  FILE *bmpImage = NULL;
  unsigned char *imgBuffer;

  // Open BPM image
  bmpImage = fopen("/home/APPS/ohyeah/shared/emv_enter_pin.bmp", "r");

  if (bmpImage != NULL) {
    // Get's the BMP lenght
    fseek(bmpImage, 0L, SEEK_END);
    fileSize = ftell(bmpImage);
    fseek(bmpImage, 0L, SEEK_SET);

    // Check if the size is higher than 0
    if (fileSize <= 0) {
      return;
    }
    // Create buffer to load the image
    imgBuffer = (char *) malloc(fileSize);
    // Load BMP image
    fread(imgBuffer, fileSize, 1, bmpImage);
    // Shows BPM into the screen
    imgDisplay(imgBuffer, fileSize);
    // Release memory
    free(imgBuffer);
    fclose(bmpImage);
  }
}

int bcPinEntry (const char *message, unsigned long long amount, unsigned int digits) {
  char pin[12+1]={0x00};
  char msg[256]={0x00};

  //OSL_Warning("bcPinEntry [%s][%llu][%d]", message, amount, digits);

  // goalClearScreen(TRUE);
  showPinImage();

  memcpy(pin, "************", digits);

  if (message == NULL) {
    sprintf(msg, "VALOR: %llu,%02llu\nSENHA: %s", (amount / 100), (amount % 100), pin);
  } else {
    sprintf(msg, "%s%s", message, pin);
  }

  formatAndPrintDisplay(msg, 2, 0);
  return 0;
}

void bcSetLeds (ppLeds_t leds) {
  //OSL_Warning("bcSetLeds");
}

void bcBeep(ppBeepType_t beepType) {
  //OSL_Warning("bcBeep");
}

void bcGetAidData (const char *aid) {
  //OSL_Warning("bcGetAidData");
}

#endif

int APPBC_CALLBACK_iDisplay(char *pszMsg)
{
  mrb_value ret;
  ContextLog(current_mrb, 1, "msg [%s]", pszMsg);

  ret = mrb_funcall(current_mrb, current_klass, "internal_text_show", 3,
      mrb_nil_value(), mrb_str_new_cstr(current_mrb, pszMsg), mrb_nil_value());

  return 0;
}

int APPBC_CALLBACK_iDisplayClear(void)
{
  ContextLog(current_mrb, 1, "APPBC_CALLBACK_iDisplayClear");
  display_clear();
  return 0;
}

int APPBC_CALLBACK_iGetPIN(char *pszMsg, int iNum)
{
	mrb_value ret;

  ContextLog(current_mrb, 1, "internal_get_pin [%s][%d]", pszMsg, iNum);

	ret = mrb_funcall(current_mrb, current_klass, "internal_get_pin", 2,
			mrb_str_new_cstr(current_mrb, pszMsg), mrb_fixnum_value(iNum));

	return mrb_fixnum(ret);
}

int APPBC_CALLBACK_iMenu(char *pszTitle, char **pvszItem, int iItem, int iTimeout)
{
  char menu_string[200];
  mrb_value ret;
  char buf[50];
  int i = 0;

  memset(menu_string, 0, sizeof(menu_string));

  ContextLog(current_mrb, 1, "title [%s] item [%s] item n [%d] timeout [%d]", pszTitle, pvszItem, iItem, iTimeout);

  for (i = 0; i < iItem; i++) {
    memset(buf, 0, sizeof(buf));
    sprintf(&buf[0], "%s\r", i+1, pvszItem[i]);
    strcat(menu_string, buf);
  }

  ret = mrb_funcall(current_mrb, current_klass, "internal_menu_show", 2,
      mrb_str_new_cstr(current_mrb, menu_string), 	mrb_str_new_cstr(current_mrb, pszTitle));

  return mrb_fixnum(ret);
}

int APP_iTestCancel(void)
{
  return 0;
}

  static mrb_value
mrb_emv_s_open(mrb_state *mrb, mrb_value klass)
{
#ifdef __FRAMEWORK_TELIUM_PLUS__
  current_mrb   = mrb;
  current_klass = klass;

  int err = PP_SetCallbacks((ppCallbacks_t) {
    .callbackVersion = PP_CALLBACK_VERSION,
    .showMenu = bcShowMenu,
    .showMessage = bcShowMessage,
    .showPinEntry = bcPinEntry,
    .setLeds = bcSetLeds
    // .setLeds = bcSetLeds,
    // .beep = bcBeep,
    // .getAidData = bcGetAidData
  });
  //OSL_Warning("mrb_emv_s_open: PP_SetCallbacks = %d", err);

  // Ativa os canais de log. A constante 0x8F00 ÃƒÂ© o SAP usado no trace.
  logSetChannels(LOG_ALL_CHANNELS, 1, &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);
  logSetChannels(0, 1, LOG_DEBUG, &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);
  logSetChannels(LOG_CH_BASE, 1, LOG_DEBUG, &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);
  abecsLogSetChannels(LOGCH_ABECS, LOGCH_ABECS_COUNT, LOG_DEBUG,
  &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);
  pinpadLogSetChannels(LOGCH_PINPAD, LOGCH_PINPAD_COUNT, LOG_DEBUG,
  &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);
  pinpadLogSetChannels(LOGCH_PINPAD_EWL_START, LOGCH_PINPAD_EWL_COUNT, LOG_DEBUG,
  &logWriteTeliumTrace, &logDumpFormattedAscii, (void *) 0x8F00);

  //OSL_Warning("Liga os logs da BC");

  err = PP_Open();
  //OSL_Warning("mrb_emv_s_open: PP_Open = %d", err);

  return mrb_fixnum_value(err);
#elif PAX
  mrb_value com;
	PPInitLibStru stInitLibStru;

  mrb_get_args(mrb, "S", &com);

  current_mrb   = mrb;
  current_klass = klass;

	memset (&stInitLibStru, '\0', sizeof (PPInitLibStru));
	stInitLibStru.iSize = sizeof (PPInitLibStru);
	stInitLibStru.stFuncs.piDisplay = APPBC_CALLBACK_iDisplay;
	stInitLibStru.stFuncs.piDisplayClear = APPBC_CALLBACK_iDisplayClear;
	stInitLibStru.stFuncs.piGetPIN = APPBC_CALLBACK_iGetPIN;
	stInitLibStru.stFuncs.piMenu = APPBC_CALLBACK_iMenu;
	stInitLibStru.stFuncs.piTestCancel = APP_iTestCancel;
	PP_iInitLib (&stInitLibStru);

  return mrb_fixnum_value(PP_Open(RSTRING_PTR(com)));
#else // GERTEC
  mrb_value com;
  DSP_Callback_Stru dsp_st;

  mrb_get_args(mrb, "S", &com);

  current_mrb   = mrb;
  current_klass = klass;

  PP_InitLib();

  dsp_st.pClear = &GDSP_Clear;
  dsp_st.piMenuStart = &GDSP_iMenuStart;
  dsp_st.piMenuShow = &GDSP_iMenuShow;
  dsp_st.pText = &GDSP_Text;
  /*Callbacks*/
  PP_DspCallbacks(&dsp_st);

  return mrb_fixnum_value(PP_Open(RSTRING_PTR(com)));
#endif
}

  static mrb_value
mrb_emv_s_close(mrb_state *mrb, mrb_value klass)
{
  int err;
  mrb_value msg;
  mrb_get_args(mrb, "S", &msg);

  err = PP_Close(RSTRING_PTR(msg));
  //OSL_Warning("mrb_emv_s_close: PP_Close = %d", err);
  return mrb_fixnum_value(err);
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
  mrb_int ret;

  //OSL_Warning("mrb_emv_s_start_get_card");

  mrb_get_args(mrb, "S", &value);

  ret = PP_StartGetCard(RSTRING_PTR(value));
  //OSL_Warning("mrb_emv_s_start_get_card: PP_StartGetCard = %d", ret);
#ifdef __FRAMEWORK_TELIUM_PLUS__
  Telium_Ttestall(0, 50);
#endif
  return mrb_fixnum_value(ret);
}

  static mrb_value
mrb_emv_s_get_card(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
  mrb_value array;
  mrb_int ret;

  //OSL_Warning("mrb_emv_s_get_card");

  //OSL_Warning("mrb_emv_s_start_get_card: PP_GetCard [%s][%s]", output, msg);
  ret = PP_GetCard(output, msg);
#ifdef __FRAMEWORK_TELIUM_PLUS__
  Telium_Ttestall(0, 50);
#endif
  //OSL_Warning("mrb_emv_s_get_card: PP_GetCard = %d", ret);

  array  = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));
  if (ret == PPCOMP_OK)
    mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, output));
  else
    mrb_ary_push(mrb, array, mrb_nil_value());
  if (ret == PPCOMP_NOTIFY) mrb_ary_push(mrb, array, mrb_str_new_cstr(mrb, msg));

  //OSL_Warning("/mrb_emv_s_get_card");

  return array;
}

  static mrb_value
mrb_emv_s_start_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  mrb_int ret;
  mrb_value process, tags, optional_tags;

  mrb_get_args(mrb, "SSS", &process, &tags, &optional_tags);

  //OSL_Warning("[mrb_emv_s_start_go_on_chip]", process);

  //OSL_Warning("psInput = [%s]", RSTRING_PTR(process));
  //OSL_Warning("psTags = [%s]", RSTRING_PTR(tags));
  //OSL_Warning("psTagsOpt = [%s]", RSTRING_PTR(optional_tags));

  ret = PP_StartGoOnChip(RSTRING_PTR(process), RSTRING_PTR(tags), RSTRING_PTR(optional_tags));
#ifdef __FRAMEWORK_TELIUM_PLUS__
  Telium_Ttestall(0, 50);
#endif

  return mrb_fixnum_value(ret);
}

  static mrb_value
mrb_emv_s_go_on_chip(mrb_state *mrb, mrb_value klass)
{
  OUTPUT output[1024]={0x00}, msg[33]={0x00};
  mrb_value array;
  mrb_int ret;

  current_mrb   = mrb;
  current_klass = klass;

  ret = PP_GoOnChip(output, msg);
#ifdef __FRAMEWORK_TELIUM_PLUS__
  Telium_Ttestall(0, 50);
#endif

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

  //OSL_Warning("PP_FinishChip [%d][%s]", ret, output);

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
  OUTPUT output[11]={0x00};
  mrb_value acquirer, array;
  mrb_get_args(mrb, "S", &acquirer);

  ret = PP_GetTimeStamp(RSTRING_PTR(acquirer), output);

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_fixnum_value(ret));

  if (ret == PPCOMP_OK) mrb_ary_push(mrb, array, mrb_str_new(mrb, output, 10));

  return array;
}

  void
mrb_bc_emv_init(mrb_state* mrb)
{
  struct RClass *plt, *emv;

  plt = mrb_define_class(mrb, "EMVPlatform", mrb->object_class);
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

