PRAGMA foreign_keys = ON;
DROP TABLE IF EXISTS tasks;
CREATE TABLE tasks (
  id INTEGER PRIMARY KEY,
  title TEXT NOT NULL,
  done INTEGER NOT NULL DEFAULT 0,
  created_at TEXT NOT NULL DEFAULT (datetime('now'))
);
INSERT INTO tasks (title, done) VALUES
  ('Write Fun + SQLite example', 1),
  ('Ship optional feature flag', 0),
  ('Celebrate with coffee', 0);
