CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
-- For distances
CREATE EXTENSION IF NOT EXISTS cube;
CREATE EXTENSION IF NOT EXISTS earthdistance;
-- TODO Add POSTGIS
SET TIME ZONE 'UTC';

create table app_users (
  id uuid default uuid_generate_v4() UNIQUE not null PRIMARY key,
  full_name not null string,
  role not null string,
  created_at timestamptz not null default current_timestamp,
  updated_at timestamptz not null default current_timestamp
);
