        FileLike *inode;
        ErrnoCode rc = resolvePath("/home/all/ztane/janna.txt", inode);
        kout << rc << endl;
        if (rc == NOERROR) {
                char buf[256];
                FileDescriptor *fd;
                FileDescriptor::open(*inode, FileDescriptor::READ, fd);
                size_t bytes_read;
                fd->read(buf, 255, bytes_read);
                buf[bytes_read] = 0;
                kout << buf;
        }

