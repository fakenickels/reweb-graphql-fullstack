let runWithConnection = (fn) => {
  let connection = PGOCaml.connect();
  let ret = fn(connection)
  PGOCaml.close(connection);
  ret
}
