#include "postgresql.h"

PostgreSQL::PostgreSQL(string username, string password, string dbname, string host){
    try {
        std::stringstream connectionParameters;
        connectionParameters << "dbname = " << dbname << " user = " << username <<  " password = " << password <<  " hostaddr = " << host <<  " port = 5432";
        cout << "connectionParameters : " << connectionParameters.str() << endl;
        C = new connection(connectionParameters.str());
    }
    catch (const exception &e) {
        cerr << e.what() << std::endl;
        throw string("Erreur Fail to connect.");
    }

    if (C->is_open()){
        cout << "Connection OK" << endl;
        C->disconnect();
    }
    else {
        cout << "Connection FAIL" << endl;
    }

    stringstream s;
    s << "CREATE TABLE IF NOT EXISTS threats ( id SERIAL primary key, name TEXT, face_descriptor float[] NOT NULL);";
    this->exec(s.str());
}

bool PostgreSQL::exec(string sql){
    work* W = new work(*C);

    try{
        W->exec(sql);
        W->commit();
        cout << "SQL exec Success" << endl;
        return true;
    }
    catch (const exception &e) {
        cerr << "EXEC ERR: " << e.what() << endl;
        return false;
    }

    return false;
}

int PostgreSQL::addThreat(string name, matrix<float,0,1> &face_descriptor){
    int x, y;
    stringstream s;

    for (long r = 0; r < face_descriptor.nr(); ++r) {
        for (long c = 0; c < face_descriptor.nc(); ++c) {
            if (r){
                s << ",";
            }
            s << face_descriptor(r,c);
        }
    }

    stringstream sql;

    sql << "(name, face_descriptor) VALUES ('"
    << name <<
    "', '{" << s.str() << "}'" <<
    ")";

    return this->insert("threats", sql.str());
}

int PostgreSQL::insert(string table, string sql){
    talbeInsert = table;
    stringstream s;

    s << "INSERT INTO " << table << sql;

    if(this->exec(s.str())){
        cout << "SUCCESSFULLY INSERTED THREAT INTO DB !!!" ;
        insertSuccess = true;
    }
    else {
        cerr << "FAIL TO INSERT THREAT INTO DB !!!";
        insertSuccess = false;
    }

    return insertSuccess;
}
