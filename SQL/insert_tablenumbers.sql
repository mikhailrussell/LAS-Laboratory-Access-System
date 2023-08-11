DELETE FROM current_session;

INSERT INTO current_session (TABLE_NUM, STDNUM, DATE, TIME)
VALUES
  (1, NULL, NULL, NULL),
  (2, NULL, NULL, NULL),
  (3, NULL, NULL, NULL),
  (4, NULL, NULL, NULL),
  (5, NULL, NULL, NULL),
  (6, NULL, NULL, NULL),
  (7, NULL, NULL, NULL),
  (8, NULL, NULL, NULL),
  (9, NULL, NULL, NULL),
  (10, NULL, NULL, NULL);

  SELECT * FROM current_session;