#include "open62541.h"
#include "sqlite3.h"
#include "sqlite3_operate.h"
#include "opcua_operate.h"


int do_insert(char *database, char *name, int value)
{
	const char *sql_create_table = "create table meachine1 (name char, value Integer, time char);";
	sqlite3 *db;
	char *errmsg;
	int ret = sqlite3_open(DATABASE, &db);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Cannot open db:\n%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else
		printf("Open database seuccess.\n");
	if (sqlite3_exec(db, sql_create_table, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
		printf("Open database seuccess.\n");
    char timestampString[50];
	UA_Int64 tOffset = UA_DateTime_localTimeUtcOffset();
	UA_DateTimeStruct dts = UA_DateTime_toStruct(UA_DateTime_now() + tOffset);
	sprintf(timestampString,"%d-%d-%d %d:%d:%d", dts.year, dts.month, dts.day, dts.hour, dts.min, dts.sec);

	char sql[100];
	sprintf(sql, "insert into meachine1 values('%s', '%d', '%s');", name, value, timestampString);

	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
		printf("Insert success.\n");
	sqlite3_close(db);
	return 0;
}

