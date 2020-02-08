type role =
  | Admin
  | User;

type t = {
  id: string,
  name: string,
  role,
};

let convertFromObject = user => {
  id: user#id,
  name: user#full_name,
  role: switch(user#role) {
  | "ADMIN" => Admin
  | "USER" => User
  | _ => User
  }
}

let findAll = () => {
  Database.runWithConnection(connection => [%pgsql.object connection("select * from app_users")])
    |> List.map(convertFromObject)
}
