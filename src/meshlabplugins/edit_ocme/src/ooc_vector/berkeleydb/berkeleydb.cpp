
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/memory_debug.h"
#include "berkeleydb.hpp"

#include "../utils/logging.h"

#ifndef NO_BERKELEY


// File: BerkeleyDb.cpp

// Class constructor. Requires a path to the location
// where the database is located, and a database name
BerkeleyDb::BerkeleyDb(const std::string &path, std::string &dbName, u_int32_t _cFlags,
           bool isSecondary, unsigned int pagesize)
    : _db(NULL, 0),               // Instantiate Db object
      dbFileName_(path + dbName), // Database file name
      cFlags_(_cFlags)          // If the database doesn't yet exist,
                                  // allow it to be created.
{
    try
    {
        // Redirect debugging information to std::cerr
        _db.set_error_stream(&std::cerr);

        // If this is a secondary database, support
        // sorted duplicates
        if (isSecondary)
            _db.set_flags(DB_DUPSORT);

		
		_db.set_pagesize(pagesize);
		//_db.set_msgfile(fout);
		//_db.set_bt_minkey(4);
        // Open the database
//		_db.open(NULL, dbFileName_.c_str(), NULL, DB_BTREE, cFlags_, pagesize);

//		_db.set_h_ffactor(4);DB_HASH DB_QUEUE
		_db.open(NULL, dbFileName_.c_str(), NULL, DB_HASH, cFlags_, pagesize);

 	}
    // DbException is not a subclass of std::exception, so we
    // need to catch them both.
    catch(DbException &e)
    {
        std::cerr << "Error opening database: " << dbFileName_ << "\n";
        std::cerr << e.what() << std::endl;
    }
    catch(std::exception &e)
    {
        std::cerr << "Error opening database: " << dbFileName_ << "\n";
        std::cerr << e.what() << std::endl;
    }
}

// Private member used to close a database. Called from the class
// destructor.
void
BerkeleyDb::close()
{
    // Close the db
    try
    {
        _db.close(0);
        std::cout << "Database " << dbFileName_
                  << " is closed." << std::endl;
    }
    catch(DbException &e)
    {
            std::cerr << "Error closing database: " << dbFileName_ << "\n";
            std::cerr << e.what() << std::endl;
    }
    catch(std::exception &e)
    {
        std::cerr << "Error closing database: " << dbFileName_ << "\n";
        std::cerr << e.what() << std::endl;
    }
}

#endif
