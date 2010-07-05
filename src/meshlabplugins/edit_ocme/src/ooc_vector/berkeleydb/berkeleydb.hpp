
#ifndef NO_BERKELEY

#ifndef BERKELEYDB_H
#define BERKELEYDB_H

#include <string>
#include <db_cxx.h>

class BerkeleyDb
{
public:
    // Constructor requires a path to the database,
    // and a database name.
    BerkeleyDb(const std::string &path, std::string &dbName,u_int32_t _cFlags,
         bool isSecondary = false,unsigned pagesize = 512);

    // Our destructor just calls our private close method.
    ~BerkeleyDb() { close(); }

    inline Db &getDb() {return _db;}

private:
    Db _db;
    std::string dbFileName_;
    u_int32_t cFlags_;

    // Make sure the default constructor is private
    // We don't want it used.
    BerkeleyDb() : _db(NULL, 0) {}

    // We put our database close activity here.
    // This is called from our destructor. In
    // a more complicated example, we might want
    // to make this method public, but a private
    // method is more appropriate for this example.
    void close();
};


#endif

#endif // NO_BERKELEY
