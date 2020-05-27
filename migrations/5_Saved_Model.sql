upgrade:
    CREATE TABLE SavedModels(id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TEXT, model_data TEXT);

downgrade:
    DROP TABLE IF EXISTS SavedModels;
    