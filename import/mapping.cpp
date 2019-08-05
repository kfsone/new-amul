#ifndef _MSC_VER
static const int MMAP_FLAGS = MAP_PRIVATE | MAP_FILE |
#    ifdef MAP_POPULATE
                              MAP_POPULATE |
#    endif
#    ifdef MAP_DENYWRITE
                              MAP_DENYWRITE |
#    endif
#    ifdef MAP_NOCACHE
                              MAP_NOCACHE |
#    endif
                              0;
#endif

error_t
NewFileMapping(const char *filepath, void **datap, size_t size)
{
    REQUIRE(filepath && datap && size);
    REQUIRE(*datap == nullptr);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
        return ENOENT;

#if defined(_MSC_VER)
    HANDLE osfh = (HANDLE)_get_osfhandle(fd);
    HANDLE maph = CreateFileMapping(osfh, NULL, PAGE_READONLY, 0, 0, NULL);
    close(fd);
    if (!maph) {
        afatal("Unable to map file %s", filepath);
    }
    void *data = MapViewOfFile(maph, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(maph);
#else
    void *data = mmap(nullptr, size, PROT_READ, MMAP_FLAGS, fd, 0);
    if (data == MAP_FAILED) {
        afatal("Failed to load file %s: %d: %s", filepath, errno, strerror(errno));
    }
    close(fd);
#endif

    if (data == nullptr)
        afatal("Unable to load file %s", filepath);

    *datap = data;

    return 0;
}

void
CloseFileMapping(void **datap, size_t length)
{
    if (datap && *datap) {
#if defined(_MSC_VER)
        BOOL result = UnmapViewOfFile(*datap);
        if (!result)
            afatal("Error closing file mapping");
#else
        munmap(*datap, length);
#endif
    }
    if (datap)
        *datap = nullptr;
}

