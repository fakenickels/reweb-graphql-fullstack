open ReWeb;

let serve = (context, schema, request) => {
  let%lwt body = request |> Request.body_string;

  let bodyJson = Yojson.Basic.from_string(body);

  let graphQLQuery =
    Yojson.Basic.Util.(bodyJson |> member("query") |> to_string);
  let graphQLVariables =
    Yojson.Basic.Util.(bodyJson |> member("variables") |> to_assoc);
  let graphQLOperationName =
    Yojson.Basic.Util.(
      bodyJson |> member("operationName") |> to_string_option
    );

  let query = Graphql_parser.parse(graphQLQuery);

  print_newline();
  print_endline("GraphQL:");
  switch (graphQLOperationName) {
  | Some("IntrospectionQuery") =>
    print_endline(" Receiving introspection query")
  | _ =>
    print_endline(" " ++ graphQLQuery);
  };

  let%lwt response =
    switch (query) {
    | Ok(query) =>
      let ctx = context();
      let graphQLVariables = (
        graphQLVariables :> list((string, Graphql_parser.const_value))
      );
      let%lwt result =
        Graphql_lwt.Schema.execute(
          schema,
          ctx,
          ~variables=graphQLVariables,
          query,
        );
      (
        switch (result) {
        | Ok(`Response(data)) => data |> Yojson.Basic.to_string
        | Ok(`Stream(_)) => {j|{}|j}
        | Error(err) => err |> Yojson.Basic.to_string
        }
      )
      |> Lwt.return;
    | Error(err) => {j|{"errors": [ |j} ++ err ++ {j| ]}|j} |> Lwt.return
    };

  response
  |> Response.of_text(~headers=[("content-type", "application/json")])
  |> Lwt.return;
}
