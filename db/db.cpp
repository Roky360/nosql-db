#include "db.h"

namespace db {
    DB::~DB() {
        delete dal;
    }

    void DB::open(const string &path, Options& options) {
        this->dal = new Dal(path, options);
    }

    void DB::close() {
        delete dal;
        dal = nullptr;
    }


}
