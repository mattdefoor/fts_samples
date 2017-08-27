#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fts.h>

void indent(int i)
{
    for (; i > 0; i--)
    {
        printf("  ");
    }
}

int compare(const FTSENT **one, const FTSENT **two)
{
    return strcmp((*one)->fts_name, (*two)->fts_name);
}

int dir_list(const char* path)
{
    int result = 0;
    FTS *fts = NULL;
    FTSENT *p = NULL;

    // Cast needed because fts_open() takes a "char * const *", instead
    // of a "const char * const *", which is only allowed in C++. fts_open()
    // does not modify the argument.
    char *files[] = { (char *)path, NULL };

    fts = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, &compare);
    if (!fts)
    {
        fprintf(stderr, "%s: fts_open failed: %s\n", path, strerror(errno));
        return -1;
    }

    while ((p = fts_read(fts)) != NULL)
    {
        switch (p->fts_info)
        {
            case FTS_D:
            case FTS_F:
            case FTS_SL:
                indent(p->fts_level);
                printf("%s\n", p->fts_name);
                break;
            default:
                break;
        }
    }
    fts_close(fts);

    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        exit(1);
    }

    int result = dir_list(argv[1]);
    if (result < 0)
    {
        exit(1);
    }

    return 0;
}

