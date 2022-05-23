#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "my_debug.h"
#include "path_utils.h"


void path_set_root(char* path)
{
    path[0] = '/';
    path[1] = '\0';
}


bool path_is_root(const char* path)
{
    MY_ASSERT(path != 0);
    return ((path[0] == '/') && (path[1] == '\0'));
}


// Given an input path, return its parent path
//  path (in)   parent (out)    return
//  ""          n/a             false
//  /           /               false
//  /abc        /               true
//  /abc/       /               true
//  /abc/def    /abc            true
//  /abc/def/   /abc            true
//  abc         /               false
bool path_get_parent(const char* path, char* parent)
{
    int len = strlen(path);
    if (len <= 0)                       // empty
    {
        return false;
    }
    if (len == 1 && '/' == path[0])     // "/"
    {
        path_set_root(parent);
        return false; 
    }
    int i = len - 1;
    if (path[i] == '/')                 // "/abc/"
        --i;
    for (; i >= 0; --i)
    {
        if (path[i] == '/')
            break;
    }
    if (i < 0)                          // "abc"
    {
        path_set_root(parent);
        return false;
    }
    if (i == 0)                         // '/abc'
    {
        path_set_root(parent);
        return true;
    }
    strncpy(parent, path, i);
    parent[i] = '\0';
    return true;
}


// Get the leaf part of a complete path
//  path (in)   leaf (out)      return
//  ""          n/a             false
//  /           n/a             false
//  //          ""              false
//  /abc/       abc             true
//  /abc/efg    efg             true
bool path_get_leaf(const char* path, char* leaf)
{
    int i = strlen(path);
    if (i <= 0)                         //  ""
        return false;
    if (i == 1 && path[0] == '/')       // "/"
        return false;
    // copy over first
    strcpy(leaf, path);
    // remove last '/'
    while (i >= 1 && leaf[i - 1] == '/')
    {
        leaf[i - 1] = '\0';
        --i;
    }
    if (i == 0)                         // "//"
        return false;
    char* slash = strrchr(leaf, '/');
    if (slash) strcpy(leaf, slash + 1);
    return true;
}


// concatenate parent + '/' + leaf -> out
// if debracket is true, remove [] from leaf before concatenating
// return false if the result is longer than len or other error
bool path_concatenate(const char* parent, const char* leaf, char* out, int len, bool debracket)
{
    // check if we need to remove square bracket from leaf
    int l = strlen(leaf);
    int p = strlen(parent);
    if (l == 0 || p == 0)
        return false;
    if (debracket)
    {
        if (!((l >= 3) && (leaf[0] == '[') && (leaf[l - 1] == ']')))
            debracket = false;
    }
    if (path_is_root(parent))
    {
        if (1 + l - (debracket ? 2 : 0) > len - 1)  // length of '/' + leaf
            return false;
        strcpy(out, "/");
    }
    else
    {
        if (p + (parent[p - 1] == '/' ? 0 : 1) + l - (debracket ? 2 : 0) > len - 1)  // length of parent + '/' + leaf
            return false;
        strcpy(out, parent);
        if (out[p - 1] != '/')
        {
            out[p] = '/';
            out[p + 1] = '\0';
        }
    }
    if (!debracket)
    {
        strcat(out, leaf);
    }
    else
    {
        int c = strlen(out) - 1;
        for (int i = 1; i < l - 1; ++i)
            out[c + i] = leaf[i];
        out[c + l - 1] = '\0';
    }
    return true;
}


// Given a file name, extract extention. Return false if no extension found
bool path_get_ext(const char* file, char* ext, int len)
{
    int l = strlen(file);
    if (l <= 0) return false;
    char* dot = strrchr(file, '.');
    if ((dot == 0) || (*(dot + 1) == '\0'))
    {
        return false;
    }
    strncpy(ext, dot + 1, len - 1);
    ext[len  - 1] = '\0';
    return true;
}


// copy path
void path_copy(const char* src, char* dest, int len)
{
    strncpy(dest, src, len - 1);
    dest[len - 1] = '\0';
}


// trim non-printable character at the back of path
bool path_trim_back(char* path)
{
    bool r = false;
    int len = strlen(path);
    for (; len > 0; --len)
    {
        if (isprint(path[len - 1]))
            break;
        path[len - 1] = '\0';
        r = true;
    }
    return r;
}
