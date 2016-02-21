#ifndef MYSQL_H
#define MYSQL_H

#include <stdio.h>
#include <mysql.h>
#include <iostream>

class Mysql
{
    public:
    Mysql()
    {
        mysql_init(&mysql);
    
        if (!mysql_real_connect(&mysql, "localhost", "root", "root", "testdb", 0, NULL, 0))
        {
            printf("Error connecting to Mysql!\n");
        }
        else
        {
            printf("Connected Mysql successful!\n");
        }
    }

    ~Mysql()
    {
        mysql_close(&mysql);
    }
    
    bool MysqlInsert(std::string query)
    {
        flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query));
        if (flag)
        {
            printf("Insert data failure!\n");
            return false;
        }
        return true;
    }
    private:
    MYSQL mysql;
};

#endif
