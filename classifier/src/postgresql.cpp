#include "postgresql.h"

void PostgreSQL::SetConnection(char *username, char *host, char *password, char *dbname) {
   this->conn=new pqxx::connection("dbname = samaritain user = samaritain password = samaritain hostaddr = 127.0.0.1 port = 5432");
}


//
// int main(int argc, char* argv[]) {
//    PostgreSQL psql;
//    psql.init("dbname = samaritain user = samaritain password = samaritain hostaddr = 127.0.0.1 port = 5432");
// }
