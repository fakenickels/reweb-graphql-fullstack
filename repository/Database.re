module Lwt_thread = {
  include Lwt;
  include Lwt_io;

  type in_channel = Lwt_io.input_channel;
  type out_channel = Lwt_io.output_channel;

  let close_in = Lwt_io.close;
  let really_input = Lwt_io.read_into_exactly;
  let input_binary_int = input_binary_int;
  let output_binary_int = output_binary_int;
  
  type input_channel = Lwt_io.input_channel;
  type output_channel = Lwt_io.output_channel;

  let input_char = Lwt_io.read_char
  let output_char = Lwt_io.write_char

  let output_string = Lwt_io.write_chars;
};

module PGOCaml = PGOCaml_generic.Make(Lwt_thread)

let runWithConnection = (fn) => {
  let connection = PGOCaml.connect();

  let ret = fn(connection)
  PGOCaml.close(connection);
  ret
}

let () = {
  Sys.set_signal(Sys.sigquit, Signal_handle((_) => {
    print_endline("Closing database connection");
    /* PGOCaml.close(connection) */
  }))
}
