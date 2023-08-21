-- Update rows in current_session with data from bookings
UPDATE current_session AS cs
JOIN bookings AS b ON cs.TABLE_NUM = b.TABLE_NUM
SET cs.STDNUM = CASE
  WHEN b.DATE = CURDATE()
    AND b.TIME >= NOW() - INTERVAL 3 HOUR
    AND b.TIME <= NOW() - INTERVAL 2 HOUR
  THEN b.STDNUM
  ELSE ''
END;