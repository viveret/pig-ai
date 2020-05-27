upgrade:
    CREATE TABLE TrainingDataWords(sentence_id INTEGER, sentence_offset INTEGER, feature_index INTEGER);

downgrade:
    DROP TABLE IF EXISTS TrainingDataWords;
    