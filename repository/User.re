let findAll = () => {
  Database.runWithConnection(connection => [%pgsql.object connection("select * from app_users")])
}
