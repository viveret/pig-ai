upgrade:
    CREATE TABLE ConfigState(id INTEGER PRIMARY KEY AUTOINCREMENT, handle TEXT UNIQUE, value TEXT);

    INSERT INTO ConfigState(handle, value) VALUES('categories', 'police,nat-guard,armed-citizen,unarmed-citizen,other');

downgrade:
    DROP TABLE IF EXISTS ConfigState;
    