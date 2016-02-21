/*
 * Author: LiHe
 * Created Time: 2016/1/30 9:05:05
 * File Name: test.c
 */
#include <mysql.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <time.h>

static char *server_options[] = { "mysql_test", "--defaults-file=my.cnf" };  
int num_elements = sizeof(server_options)/ sizeof(char *);  
   
static char *server_groups[] = { "libmysqld_server", "libmysqld_client" };  

const int MAXN = 100000;

int main(void)  
{  
    
    char buf[256] = "insert into t_users values(";   
    if (mysql_server_init(num_elements, server_options, server_groups))  
    {  
        exit(1);  
    }  
   
    MYSQL *conn;  
    MYSQL_RES *res;  
    MYSQL_ROW row;  
    char *server = "localhost";  
    char *user = "root";  
    char *password = "root";  
    char *database = "testdb";  
    conn = mysql_init(NULL);  
   
    /* Connect to database */  
    if (!mysql_real_connect(conn, server,  
                            user, password, database, 0, NULL, 0))  
    {  
        fprintf(stderr, "%s/n", mysql_error(conn));  
        exit(1);  
    }  
   
    /* send SQL query */  
    //insert   
    time_t a = time(0);
    for (int i = 0; i < MAXN; i++)
    {
	int a = i, tot = 27;
	while (a)
	{
	    buf[tot++] = a % 10 + '0';
	    a /= 10;
	}
        buf[tot++] = ',';
	buf[tot++] = '\'';
	buf[tot++] = 'H';
	buf[tot++] = '\'';
	buf[tot++] = ')';
	int res = mysql_query(conn, buf);
    }
    time_t b = time(0);
    int c = b - a;
    printf("insert: %.2lf/s\n", MAXN * 1.0 / c);
    //update   

    a = time(0);
    for (int i = 0; i < MAXN; i++)
    {
        int res = mysql_query(conn, "update t_users set username = 'LiHe' where userid = 000005");
    }
    b = time(0);
    c = b - a;
    printf("update: %.2lf/s\n", MAXN * 1.0 / c);

    //delete
    a = time(0);
    for (int i = 0; i < MAXN; i++)
    {
        int res = mysql_query(conn, "delete from t_users where userid = 000005");
    }
    b = time(0);
    c = b - a;
    printf("delete: %.2lf/s\n", MAXN * 1.0 / c);

    /* close connection */  
    mysql_free_result(res);  
    mysql_close(conn);  
    /* Use any MySQL API functions here */  
    mysql_server_end();  
    return EXIT_SUCCESS;  
}   
