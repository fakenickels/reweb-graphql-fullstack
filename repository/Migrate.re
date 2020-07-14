type migration = {
  createdAt: int,
  script: string
}

let migrations: ref(array(migration)) = ref([||])

let setup = () => {
  Database.runWithConnection(connection => {
    [%pgsql connection("
      create table migrations (
        id uuid default uuid_generate_v4() UNIQUE not null PRIMARY key,
        name string not null,
        created_at timestamptz not null default current_timestamp
      )
    ")]
  })
}

let up = (fn) => {
  Database.runWithConnection(fn)
}
