open Graphql_lwt;

type role =
  | Admin
  | User;

type user = {
  id: int,
  name: string,
  role,
};

let users = [
  {id: 1, name: "Alice", role: Admin},
  {id: 2, name: "Bob", role: User},
];

let role: Schema.typ(unit, option(role)) =
  Schema.(
    enum(
      "role",
      ~doc="The role of a user",
      ~values=[
        enum_value("USER", ~value=User),
        enum_value("ADMIN", ~value=Admin),
      ],
    )
  );

let user =
  Schema.(
    obj("user", ~doc="A user in the system", ~fields=_ =>
      [
        field(
          "id",
          ~doc="Unique user identifier",
          ~typ=non_null(int),
          ~args=Arg.[],
          ~resolve=(_info, p) =>
          p.id
        ),
        field(
          "name", ~typ=non_null(string), ~args=Arg.[], ~resolve=(_info, p) =>
          p.name
        ),
        field("role", ~typ=non_null(role), ~args=Arg.[], ~resolve=(_info, p) =>
          p.role
        ),
      ]
    )
  );

let schema =
  Schema.(
    schema([
      field(
        "users",
        ~doc="A user",
        ~typ=non_null(list(non_null(user))),
        ~args=Arg.[],
        ~resolve=(_info, ()) =>
        [{id: 1, name: "Bob", role: Admin}]
      ),
    ])
  );
