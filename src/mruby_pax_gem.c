#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/hash.h"

#define DONE mrb_gc_arena_restore(mrb, 0)

void mrb_bc_emv_init(mrb_state* mrb);
void mrb_bc_pinpad_init(mrb_state* mrb);
void mrb_bc_table_init(mrb_state* mrb);

void
mrb_mruby_emv_gem_init(mrb_state* mrb)
{
  mrb_bc_emv_init(mrb); DONE;
  mrb_bc_pinpad_init(mrb); DONE;
  mrb_bc_table_init(mrb); DONE;
}

void
mrb_mruby_emv_gem_final(mrb_state* mrb)
{
}

