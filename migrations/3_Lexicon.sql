upgrade:
    CREATE TABLE Lexicon(id INTEGER PRIMARY KEY AUTOINCREMENT, label TEXT UNIQUE, occurrences INTEGER, freq REAL);
    --CREATE TABLE Speakers(lexicon_id INTEGER PRIMARY KEY);

    CREATE TABLE Sentences(id INTEGER PRIMARY KEY AUTOINCREMENT, source_id INTEGER, speaker_name TEXT, what_said TEXT);
    CREATE TABLE SentenceSources(id INTEGER PRIMARY KEY AUTOINCREMENT, file_path TEXT);

downgrade:
    DROP TABLE IF EXISTS Sentences;
    DROP TABLE IF EXISTS SentenceSources;
    --DROP TABLE IF EXISTS Speakers;
    DROP TABLE IF EXISTS Lexicon;
    