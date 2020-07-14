open Lwt;

let () = Lwt_main.run({
  let%lwt _ = Repository.Migrate.setup()
})
