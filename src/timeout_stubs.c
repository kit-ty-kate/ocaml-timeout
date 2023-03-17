#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define CAML_INTERNALS
#include <caml/fail.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/instrtrace.h>

struct ocaml_timeout_arg {
  intnat* icount;
  int polling_rate;
  intnat instruction_limit;
};

static void* loop(void* arg) {
  struct ocaml_timeout_arg* data = arg;
  intnat* icount = data->icount;
  int polling_rate = data->polling_rate;
  intnat instruction_limit = data->instruction_limit;

  while (*icount > -instruction_limit) {
    usleep(polling_rate);
  }

  printf("\n[TIMEOUT] Instruction limit reached.\n");
  fflush(stdout);
  exit(1);

  return NULL;
}

CAMLprim value ocaml_timeout_watch(value polling_rate, value instruction_limit) {
  CAMLparam2(polling_rate, instruction_limit);

  /* NOTE: In OCaml < 5.0 this isn't needed as caml_icount isn't thread-local.
     However, in OCaml >= 5.0 this value became thread-local so we need to
     bypass the thread-local state of this global by taking its address before
     spawning a new thread. */
  intnat* icount = &caml_icount;
  struct ocaml_timeout_arg* arg = malloc(sizeof (struct ocaml_timeout_arg));

  arg->icount = icount;
  arg->polling_rate = Int_val(polling_rate);
  arg->instruction_limit = Nativeint_val(instruction_limit);

  if (arg->polling_rate < 1 || arg->polling_rate > 1000000)
    caml_invalid_argument("polling_rate < 1 || > 1_000_000");
  if (arg->instruction_limit < 1)
    caml_invalid_argument("instruction_limit < 1");

  pthread_t thread;
  pthread_create(&thread, NULL, loop, arg);

  CAMLreturn(Val_unit);
}

CAMLprim value ocaml_timeout_get_current_instruction_count(value unit) {
  CAMLparam1(unit);
  CAMLlocal1(ret);

  ret = caml_copy_nativeint(caml_icount);

  CAMLreturn(ret);
}
