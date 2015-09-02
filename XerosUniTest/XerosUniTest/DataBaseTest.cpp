#include "Common.h"

#include "..\..\XerosApp\XerosApp\DataBase\DataBase.h"

TEST(DATABASE, SELECT_SQL)
{
	//ASSERT_EQ(15, 1);
}

TEST(DATABASE, UPDATE_SQL)
{
	ST_DB_LOGIN_TOKEN stDBLoginToken;
	stDBLoginToken.strDatabaseName = "postgres";
	stDBLoginToken.strDatabaseIP = "165.132.120.152";
	stDBLoginToken.strUserName = "postgres";
	stDBLoginToken.strPassword = "xeros";
	stDBLoginToken.strPort = "5432";

	HANDLE hDataBase = NULL;
	hDataBase = CreateDBInstance(E_DB_POSTGRES);
	ASSERT_TRUE(hDataBase != NULL);

	DWORD dwRet;
	dwRet = ConnectToDB(hDataBase, stDBLoginToken);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);

	ST_DB_SQL stDBQuerySQL;
	stDBQuerySQL.strSQL = "SELECT word FROM \"WordRelation\" WHERE word = \'test\'"; 
	ST_DB_RESULT stDBResult;
	dwRet = QueryFromDB(hDataBase, stDBQuerySQL, stDBResult);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);

	if (stDBResult.vecstrResult.size() < 1) {
		return;
	}

	ST_DB_SQL stDBSQL;
	stDBSQL.strSQL = "UPDATE \"WordRelation\" SET sequence=3 , frequency=3 WHERE word=\'test\'";
	dwRet = UpdateToDB(hDataBase, stDBSQL);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);
	
}