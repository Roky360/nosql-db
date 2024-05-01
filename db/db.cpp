#include "db.h"

namespace db {
    DB::DB() : defaultTx(new SharedTx(this)), dal(nullptr) {}

    DB::~DB() {
        delete dal;
    }

    void DB::open(const string &path, Options &options) {
        this->dal = new Dal(path, options);
    }

    void DB::close() {
        if (dal) {
            delete dal;
            dal = nullptr;
        }
    }

    ReadTx *DB::readTransaction() {
        return new ReadTx(this);
    }

    WriteTx *DB::writeTransaction() {
        return new WriteTx(this);
    }


}
