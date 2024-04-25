#include "freelist.h"
#include "../utils/ioutils.h"

using namespace ioutils;

namespace dal {
    FreeList::FreeList() {
        this->maxPage = INITIAL_PAGE;
        this->releasedPages = vector<pgnum>();
    }

    pgnum FreeList::getNextPageNum() {
        if (!this->releasedPages.empty()) {
            pgnum pnum = this->releasedPages[this->releasedPages.size()];
            this->releasedPages.pop_back();
            return pnum;
        }

        return ++this->maxPage;
    }

    void FreeList::releasePage(pgnum pageNum) {
        this->releasedPages.push_back(pageNum);
    }

    bytearray FreeList::serialize(bytearray buf) {
        size_t pos = 0;

        // write max page
        writeIntToStream(buf, pos, (uint64) this->maxPage);

        // write released pages (len and pages themselves)
        writeIntToStream(buf, pos, (uint64) this->releasedPages.size());
        for (auto i : this->releasedPages) {
            writeIntToStream(buf, pos, i);
        }

        return buf;
    }

    void FreeList::deserialize(bytearray buf) {
        size_t pos = 0;
        this->maxPage = readIntFromStream(buf, pos);

        uint64 releasedPagesCount = readIntFromStream(buf, pos);
        for (uint64 i = 0; i < releasedPagesCount; i++) {
            this->releasedPages.push_back(readIntFromStream(buf, pos));
        }
    }
}


