#ifndef _PSQL_H_
#define _PSQL_H_

#include <stack>
#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <dlib/matrix.h>

using namespace std;
using namespace pqxx;
using namespace dlib;

class PostgreSQL
{
    public:
    PostgreSQL(string user, string pass, string db, string host);
    ~PostgreSQL();

    bool exec(string sql);
    void disconnect();

    int addThreat(string name, dlib::matrix<float,0,1> &face_descriptor);
    // matrix<float> getFace(string name);
    // matrix<float> getFace(int id);

    // int lastInsertID();
    // int lastInsert();
    int insert(string table, string sql);

    private:
    connection* C;
    string talbeInsert;
    bool insertSuccess;

};

#endif
