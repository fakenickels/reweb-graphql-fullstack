let get_users = () => {
  let connection = PGOCaml.connect();
  let users = [%pgsql.object connection("select * from app_users")];
  PGOCaml.close(connection);
  users
}
