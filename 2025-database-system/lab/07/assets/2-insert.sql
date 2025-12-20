Select @i:=max(id) from test;
INSERT INTO test (rq, srq, hh, mm, ss, num)
VALUES (
	NOW(),
	NOW(),
	HOUR(NOW()),
	MINUTE(NOW()),
	SECOND(NOW()),
	RAND(@i) * 100
);
