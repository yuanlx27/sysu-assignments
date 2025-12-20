CREATE DATABASE IF NOT EXISTS jxgl;
USE jxgl;

DROP TABLE IF EXISTS test;
CREATE TABLE test (
	id INT UNIQUE AUTO_INCREMENT,
	rq DATETIME NULL,
	srq VARCHAR(20) NULL,
	hh SMALLINT NULL,
	mm SMALLINT NULL,
	ss SMALLINT NULL,
	num NUMERIC(12,3),
	PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=1;

DELIMITER //
CREATE PROCEDURE generate_tests()
BEGIN
    DECLARE i INT DEFAULT 1;
    WHILE i <= 80000 DO
        INSERT INTO test(rq, srq, hh, mm, ss, num)
        VALUES (
            NOW(),
            NOW(),
            HOUR(NOW()),
            MINUTE(NOW()),
            SECOND(NOW()),
            RAND(i) * 100
        );
        SET i = i + 1;
    END WHILE;
END//
DELIMITER ;

CALL generate_tests();
