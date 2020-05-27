upgrade:
    CREATE TABLE ConfigState(id INTEGER PRIMARY KEY AUTOINCREMENT, handle TEXT UNIQUE, value TEXT);

    INSERT INTO ConfigState(handle, value) VALUES('import_sentence_regex', '\W|\d|\w+'),
                                                ('import_word_prefix_list', 'PRE|RE|DIS|UN|IN|GOD'),
                                                ('import_word_suffix_list', 'ED|ER|ING|ION|LY|IES|NESS|IST|S'),
                                                ('import_word_nofix_list', '\w+here|\w+[^wsgm]ed|\w+ris|\w+uth?er|\w+ous|re\w+er]|\w+ether|\w*nder|\w+uly|re\w+ec?t|\w*[tw]ist|always|nothing|decker|paper|really|\w+sease|\w+us|after|red|her|ring|fly|his'),
                                                ('import_word_ignore_regex', 'bitch|prostitute');




downgrade:
    DROP TABLE IF EXISTS ConfigState;
    