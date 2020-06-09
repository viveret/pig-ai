upgrade:
    CREATE TABLE SourceImages(id INTEGER PRIMARY KEY AUTOINCREMENT, path VARCHAR UNIQUE, width INTEGER, height INTEGER, channels INTEGER, category INTEGER, data BLOB);
    CREATE TABLE TrainingImages(id INTEGER PRIMARY KEY, data BLOB);

downgrade:
    DROP TABLE IF EXISTS TrainingImages;
    DROP TABLE IF EXISTS SourceImages;
    