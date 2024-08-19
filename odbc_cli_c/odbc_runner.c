/* https://anukulverma.wordpress.com/2016/02/13/unixodbc-sample-c-program/
 * with minor edits
 * */
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <string.h>

#define ENABLE_DB_TRANSACTION

#define BUFFER_SIZE 1024

/* === Utility functions to handle error === */
#define HANDLE_ERROR(r, f, s, t)                                               \
  if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) {                        \
    extract_error(f, s, t, __LINE__, __func__);                                \
    goto exit;                                                                 \
  }

void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type, int line,
                   const char *func) {
  SQLINTEGER i = 0;
  SQLINTEGER NativeError;
  SQLCHAR SQLState[7];
  SQLCHAR MessageText[256];
  SQLSMALLINT TextLength;
  SQLRETURN ret;
  printf("\nError!!! In ‘%s’ (line no. %d) ", func, line - 1);
  printf("ODBC reported following error for %s : ", fn);
  do {
    ret = SQLGetDiagRec(type, handle, ++i, SQLState, &NativeError, MessageText,
                        sizeof(MessageText), &TextLength);
    if (SQL_SUCCEEDED(ret)) {
      printf("\n (%s %ld %ld) => %s\n\n", SQLState, (long)i, (long)NativeError,
             MessageText);
    }
  } while (ret == SQL_SUCCESS);
}

/* MAIN Routine */
int run_tests(unsigned char *dsn) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret;
  SQLLEN tableInd, columnInd, typeInd;
  SQLLEN idInd[3], nameInd[3], ageInd[3];
  char buf[BUFFER_SIZE] = {0};

  int i = 0;
  SQLCHAR table[64] = {0};
  SQLCHAR column[64] = {0};
  SQLLEN type;
  int id;
  char name[64] = {0};
  unsigned short age;
  /* ===== Connect to DB ===== */
  /* Allocate an environment handle */
  ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
  HANDLE_ERROR(ret, "SQLAllocHandle", env, SQL_HANDLE_ENV);
  /* We want ODBC 3 support */
  ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
  HANDLE_ERROR(ret, "SQLSetEnvAttr", env, SQL_HANDLE_ENV);
  /* Allocate a connection handle */
  ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
  HANDLE_ERROR(ret, "SQLAllocHandle", dbc, SQL_HANDLE_DBC);
  /* Connect to the DSN */
  ret = SQLDriverConnect(dbc, NULL, dsn, SQL_NTS, NULL, 0, NULL,
                         SQL_DRIVER_COMPLETE);
  HANDLE_ERROR(ret, "SQLDriverConnect", dbc, SQL_HANDLE_DBC);
  /* Allocate a statement handle */
  ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  /* ======== Create a table in Database ====== */
  memset(buf, 0, BUFFER_SIZE);
  sprintf(buf, "CREATE TABLE JI ( id integer, name varchar(64), age SMALLINT, "
               "PRIMARY KEY(id))");
  ret = SQLExecDirect(stmt, buf, SQL_NTS);
  if (ret != SQL_NO_DATA) {
    HANDLE_ERROR(ret, "SQLAllocHandle", stmt, SQL_HANDLE_STMT);
  }
/* ===== Insert 5 Rows ===== */
#ifdef ENABLE_DB_TRANSACTION
  ret = SQLExecDirect(stmt, "BEGIN IMMEDIATE TRANSACTION", SQL_NTS);
  if (ret != SQL_NO_DATA) {
    HANDLE_ERROR(ret, "SQLAllocHandle", stmt, SQL_HANDLE_STMT);
  }
#endif
  for (i = 1; i <= 5; i++) {
    sprintf(buf, "INSERT INTO JI (id, name, age) values (%d, ‘sp%d’, %d)", i, i,
            24 + i);
    ret = SQLExecDirect(stmt, buf, SQL_NTS);
    HANDLE_ERROR(ret, "SQLExecDirect", stmt, SQL_HANDLE_STMT);
  }
#ifdef ENABLE_DB_TRANSACTION
  ret = SQLExecDirect(stmt, "COMMIT", SQL_NTS);
  if (ret != SQL_NO_DATA) {
    HANDLE_ERROR(ret, "SQLEndTran", dbc, SQL_HANDLE_DBC);
  }
#endif
  /* ========== Fetch Info from DB ======== */
  /* ============ Fetch table info ============= */
  ret = SQLColumns(stmt, NULL, 0, NULL, 0, "JI", SQL_NTS, NULL, 0);
  HANDLE_ERROR(ret, "SQLColumns", stmt, SQL_HANDLE_STMT);
  ret = SQLBindCol(stmt, 3, SQL_C_CHAR, table, sizeof(table), &tableInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column, sizeof(column), &columnInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  ret = SQLBindCol(stmt, 5, SQL_C_LONG, &type, 0, &typeInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  printf("\n====== DB Schema ====== \n");
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    printf(" Table – %s : Column – %s : Type – %d \n", table, column, type);
  }
  printf("\n");
  /* ====== Fetch Data ====== */
  ret = SQLExecDirect(stmt, (SQLCHAR *)"SELECT * FROM JI", SQL_NTS);
  HANDLE_ERROR(ret, "SQLExecDirect", stmt, SQL_HANDLE_STMT);
  /* Bind columns (3 columns – id,name,age) */
  ret = SQLBindCol(stmt, 1, SQL_C_ULONG, &id, 0, idInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  ret = SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), nameInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  ret = SQLBindCol(stmt, 3, SQL_C_USHORT, &age, 0, ageInd);
  HANDLE_ERROR(ret, "SQLBindCol", stmt, SQL_HANDLE_STMT);
  /* Fetch and print each row of data until SQL_NO_DATA returned. */
  printf("\n====== DB Data ====== \n");
  for (i = 0;; i++) {
    ret = SQLFetch(stmt);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
      printf(" Row %d : %d %s %d\n", i + 1, id, name, age);
    } else {
      if (ret != SQL_NO_DATA) {
        HANDLE_ERROR(ret, "SQLFetch", stmt, SQL_HANDLE_STMT);
      } else {
        break;
      }
    }
  }
  printf("\n\n");
/* =========== Free handlers ============ */
exit:
  /* Free Statement handler */
  if (stmt != SQL_NULL_HSTMT)
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  /* Free Connection handler */
  if (dbc != SQL_NULL_HDBC) {
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  }
  /* Free Environment handler */
  if (env != SQL_NULL_HENV)
    SQLFreeHandle(SQL_HANDLE_ENV, env);
  return 0;
}
