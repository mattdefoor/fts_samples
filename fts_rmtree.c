#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fts.h>

int rm_tree(const char *path)
{
    int result = 0;
    FTS *fts = NULL;
    FTSENT *p = NULL;

    // Cast needed because fts_open() takes a "char * const *", instead
    // of a "const char * const *", which is only allowed in C++. fts_open()
    // does not modify the argument.
    char *files[] = { (char *)path, NULL };

    fts = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
    if (!fts)
    {
        fprintf(stderr, "%s: fts_open failed: %s\n", path, strerror(errno));
        return -1;
    }

    while ((p = fts_read(fts)) != NULL)
    {
        switch (p->fts_info)
        {
            case FTS_DNR:
            case FTS_ERR:
            case FTS_NS:
                fprintf(stderr, "%s: fts_read error: %s\n",
                    p->fts_accpath, strerror(p->fts_errno));
                break;

            case FTS_DC:
            case FTS_DOT:
            case FTS_NSOK:
                // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT
                // were passed to fts_open().
                break;

            case FTS_D:
                // Do nothing. Need depth-first search so directories are
                // deleted in FTS_DP;
                break;

            case FTS_DP:
            case FTS_F:
            case FTS_SL:
            case FTS_SLNONE:
            case FTS_DEFAULT:
                if (remove(p->fts_accpath) < 0)
                {
                    fprintf(stderr, "%s: Failed to remove: %s\n",
                        p->fts_path, strerror(errno));
                    result = -1;
                }
                break;
        }
    }
    fts_close(fts);
    return result;
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        exit(1);
    }

    int result = rm_tree(argv[1]);
    if (result < 0)
    {
        exit(1);
    }

    return 0;
}
