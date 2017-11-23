#ifndef _PSQL_H_
#define _PSQL_H_

#include <stack>
#include <string>
#include <iostream>
#include <pqxx/pqxx>

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/opencv.h>

// #include "./facerec.h"

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
    std::vector<dlib::matrix<float,0,1>> getThreats();
    std::vector<string> getThreatsNames();

    private:
    connection* C;
    string talbeInsert;
    bool insertSuccess;
};

#endif
