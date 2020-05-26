let name = "1581800760";

let up = (conn) => {
  [%pgsql conn("
    create table app_users (
      id uuid default uuid_generate_v4() UNIQUE not null PRIMARY key,
      full_name string not null,
      role string not null,
      created_at timestamptz not null default current_timestamp,
      updated_at timestamptz not null default current_timestamp
    );
  ")];
}

let down = (conn) => {
  [%pgsql conn("drop database app_users")];
}
