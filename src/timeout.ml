external watch :
  polling_rate:int ->
  instruction_limit:nativeint ->
  unit = "ocaml_timeout_watch"

external get_current_instruction_count :
  unit ->
  nativeint = "ocaml_timeout_get_current_instruction_count"
