#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

class PostgreSQL {
public:
   pqxx::connection* conn;
   char *username;
   char *host;
   char *password;
   char *dbname;

   void SetConnection(char *, char *, char *, char *);
};
