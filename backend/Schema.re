open Graphql_lwt;
open Repository;

type context = unit;

// We need to specify the type to inject the context type
let role: Schema.typ(context, option(User.role)) =
  Schema.(
    enum(
      "role",
      ~doc="The role of a user",
      ~values=[
        enum_value("USER", ~value=User.User),
        enum_value("ADMIN", ~value=User.Admin),
      ],
    )
  );

let user = {
  // local scope and open so the compiler can pick up the User.t type for the fields
  open User;

  Schema.(
    obj("user", ~doc="A user in the system", ~fields=_ =>
      [
        field(
          "id",
          ~doc="Unique user identifier",
          ~typ=non_null(string),
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
}

let schema =
  Schema.(
    schema([
      field(
        "users",
        ~doc="A user",
        ~typ=non_null(list(non_null(user))),
        ~args=Arg.[],
        ~resolve=(_info, ()) => {
          Repository.User.findAll();
        }
      ),
    ])
  );
