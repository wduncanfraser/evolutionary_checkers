#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;

int main(int argc, char* argv[])
{
    std::string set_name = "Test_set_2";
    int set_id = 0;
    std::string sql;

    try
    {
        connection C("dbname=checkers user=checkers password=rootpass hostaddr=172.16.106.140 port=5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }

        //Get Set id
        bool create_set = false;
        int gen_id;
        int gen_count;

        {
            sql = "SELECT id from Set where name like \'" + set_name + "\'";
            nontransaction N(C);
            result R(N.exec(sql));

            if (R.size() == 0) {
                create_set = true;
            }
            else {
                result::const_iterator c = R.begin();
                set_id = c[0].as<int>();
            }
        }

        if(create_set) {
            std::cout << "Set \"" + set_name + "\" not yet in database, creating. \n";
            {
                sql = "INSERT INTO Set (name) " \
                        "VALUES (\'" + set_name + "\'); ";

                work W(C);
                W.exec(sql);
                W.commit();
            }
            sql = "SELECT id from Set where name like \'" + set_name + "\'";
            nontransaction N(C);
            result R(N.exec(sql));

            result::const_iterator c = R.begin();
            set_id =c[0].as<int>();
        }

        std::cout << "Set id: " << set_id << std::endl;

        //Get last generation id and count
        {
            sql = "SELECT id, \"Count\" from \"Generation\" where \"Set_id\"=" + std::to_string(set_id) + " order by \"Count\" desc limit 1;";
            nontransaction N(C);
            result R(N.exec(sql));

            if (R.size() == 0) {
                gen_count = 0;
            }
            else {
                result::const_iterator c = R.begin();
                gen_id = c[0].as<int>();
                gen_count = c[1].as<int>();
            }
        }
        //Increment for new generation
        gen_count++;

        {
            {
                sql = "INSERT INTO \"Generation\" (\"Set_id\", \"Count\") " \
                        "VALUES (" + std::to_string(set_id) + ", " + std::to_string(gen_count) + "); ";

                work W(C);
                W.exec(sql);
                W.commit();
            }
            sql = "SELECT id, \"Count\" from \"Generation\" where \"Set_id\"=" + std::to_string(set_id) + " order by \"Count\" desc limit 1;";
            nontransaction N(C);
            result R(N.exec(sql));

            result::const_iterator c = R.begin();
            gen_id = c[0].as<int>();
        }

        std::cout << "Current Generation for set: " << gen_count << ".\nGeneration id: " << gen_id << std::endl;

        C.disconnect();
    }
    catch (const std::exception &e){
        cerr << e.what() << endl;
        return 1;
    }
}