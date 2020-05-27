#ifndef SQL_CONTEXT_H
#define SQL_CONTEXT_H

struct sqlite3;

sqlite3* SqlContext();
void SqlContext_Init();
void SqlContext_Clean();

#endif