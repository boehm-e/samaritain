#include "./headers/facerec.h"
#include "./headers/postgresql.h"

PostgreSQL::PostgreSQL(string username, string password, string dbname, string host){
  try {
    std::stringstream connectionParameters;
    connectionParameters << "dbname = " << dbname << " user = " << username <<  " password = " << password <<  " hostaddr = " << host <<  " port = 5432";
    cout << "connectionParameters : " << connectionParameters.str() << endl;
    this->C = new connection(connectionParameters.str());
  }
  catch (const exception &e) {
    cerr << e.what() << std::endl;
    throw string("Erreur Fail to connect.");
  }

  if (this->C->is_open()){
    cout << "Connection OK" << endl;
    this->C->disconnect();
  }
  else {
    cout << "Connection FAIL" << endl;
  }

  stringstream s;
  s << "CREATE TABLE IF NOT EXISTS threats ( id SERIAL primary key, name TEXT UNIQUE, face_descriptor float[] NOT NULL);";
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

std::array<float, 128> parse(string str) {
  std::array<float, 128> descriptor;
  int index = 0;
  stringstream sql;
  for (size_t i = 1; i <= str.size(); i++) {
    if ((char)str[i] == ',' || i == str.size()) {
      descriptor[index] = std::stof(sql.str());
      index++;
      sql.str("");
    } else {
      sql << str[i];
    }

  }
  // for (size_t i = 0; i < 128; i++) {
  //     cout << descriptor[i] << ",";
  // }
  return descriptor;
}

void print_descriptor(matrix<float,0,1> descriptor) {
  for (size_t i = 0; i < 128; i++) {
    cout << descriptor(0,i) << ",";
  }
}

matrix<float,0,1> to_matrix(std::array<float, 128> descriptor) {
  matrix<float,0,1> matrix_descriptor(128, 1);;
  for (size_t i = 0; i < 128; i++) {
    matrix_descriptor(0, i) = descriptor[i];
  }
  return matrix_descriptor;
}

std::vector<matrix<float,0,1>> PostgreSQL::getThreats() {
  stringstream sql;
  pqxx::work txn(*this->C);
  pqxx::result r = txn.exec("SELECT * FROM threats");

  // std::array<float, 128> threats;
  int size = 50;
  const double test2 = 5;

  std::vector<matrix<float,0,1>> threats;

  for (auto row: r) {
    std::cout
    << row["name"].c_str()
    << std::endl;

    std::array<float, 128> threat;
    threat = parse(row["face_descriptor"].c_str());
    threats.push_back(to_matrix(threat));
  }

  // print_descriptor(threats[0]);
  txn.commit();
  return threats;
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

// int main() {
//   PostgreSQL *db = new PostgreSQL("samaritain","samaritain","samaritain","127.0.0.1");
//   db->getThreats();
//   return 0;
// }
