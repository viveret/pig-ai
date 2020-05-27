upgrade:
    CREATE TABLE System(version INT, upgrading INT);
    INSERT INTO System VALUES(0, 0);

downgrade:
    DROP TABLE IF EXISTS System;
