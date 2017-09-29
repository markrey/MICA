#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/time.h>

int print_resp( void * , int , char ** , char ** );

struct timeval myTime;
struct tm *time_st;

int main( void ){

	int ret       = 0;
	sqlite3 *conn = NULL;
	char *err_msg = NULL;
	char sql_str[255];
	const char *dbname="/tmp/db_test.sqlite3";
	// stmt を生成する
	const char *sql = "insert into commdata values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
	sqlite3 *db=NULL;
	sqlite3_stmt *stmt=NULL;
	// sqlの?の部分に、値を設定
	unsigned long long int rectime;
	int rcvData[16]={123,234,345,456,567,678,789,890,901,1010,1011,1012,1013,1014,1015,1016};
	char sensName[16];
	int r;
	int usec = 0;
	int idx = 0;
	int item = 0;

	r = sqlite3_open(dbname, &db);
	if (SQLITE_OK!=r){
	   // open失敗
	}
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

	for(idx=0; idx<100; idx++){
		gettimeofday(&myTime, NULL);
		time_st = localtime(&(myTime.tv_sec));
		rectime =(time_st->tm_year+1900);
		rectime = (rectime * 100) + ((time_st->tm_mon+1));
		rectime = (rectime * 100) + ((time_st->tm_mday));
		rectime = (rectime * 100) + ((time_st->tm_hour));
		rectime = (rectime * 100) + ((time_st->tm_min));
		rectime = (rectime * 100) + ((time_st->tm_sec));
		rectime = (rectime * 1000) + ((myTime.tv_usec)/1000);

		sqlite3_reset(stmt);
		sqlite3_bind_int64(stmt, 1,  rectime);
		for(item=0; item<16; item++){
			sqlite3_bind_int(stmt, item+2,  rcvData[item]);
		}
		//sqlite3_step(stmt);
		while (sqlite3_step(stmt) == SQLITE_BUSY);	
		printf("rectime:%llu,rcvData:%d\n",rectime,rcvData[0]);
	}
	// stmt を開放
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

int print_resp(
      void *get_prm   , // sqlite3_exec()の第4引数
      int col_cnt     , // 列数
      char **row_txt  , // 行内容
      char **col_name   // 列名
    ){
  printf( "%s : %s\n" , row_txt[0] , row_txt[1] );
  return 0;
}

