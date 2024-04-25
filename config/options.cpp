#include "options.h"

#include <utility>

Options Options::defaultOptions = Options(
        DEFAULT_PAGE_SIZE,
        .5,
        .95,
        DB_DEFAULT_FILE_EXTENSION
);

Options::Options(int pageSize, float minPageFillPercent, float maxPageFillPercent, string dbFileExtension)
        : pageSize(pageSize), minPageFillPercent(minPageFillPercent), maxPageFillPercent(maxPageFillPercent),
          dbFileExtension(std::move(dbFileExtension)) {}

Options::Options(Options &o) {
    this->pageSize = o.pageSize;
    this->minPageFillPercent = o.minPageFillPercent;
    this->maxPageFillPercent = o.maxPageFillPercent;
    this->dbFileExtension = string(o.dbFileExtension);
}
