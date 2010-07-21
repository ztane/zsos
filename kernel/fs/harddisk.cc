#include "harddisk.hh"

ErrnoCode Partition::read (void *buf, size_t amount, FileOffset offset, size_t& read) {
    uint64_t remaining = size.to_scalar() - offset.to_scalar();
    if (remaining < amount) {
         amount = remaining;
    }

    return container->getDevice()->read(buf, amount, offset + this->offset, read);
}


