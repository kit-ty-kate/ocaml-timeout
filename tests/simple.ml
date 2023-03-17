let () =
  Timeout.watch ~polling_rate:1 ~instruction_limit:100000n;
  (* Wait forever to check that Timeout watch exits the program correctly *)
  while true do () done
