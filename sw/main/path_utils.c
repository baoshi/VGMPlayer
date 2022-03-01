#include <string.h>
#include <ff.h>
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
// if debracket is true, remove [] from leaf before concatenate
// return false if the result is longer than FF_LFN_BUF or other error
bool path_concatenate(const char* parent, const char* leaf, char* out, bool debracket)
{
    // check if we need to remove square bracket from leaf
    int len = strlen(leaf);
    if (len <= 0)
        return false;
    if (debracket)
    {
        if (!((len >= 3) && (leaf[0] == '[') && (leaf[len - 1] == ']')))
            debracket = false;
    }
    // check length
    if (strlen(parent) + 1 + len - (debracket ? 2 : 0) > FF_LFN_BUF)
        return false;
    if (!path_is_root(parent))
    {
        strcpy(out, parent);
        int c = strlen(out);
        if (out[c - 1] != '/') strcat(out, "/");
    }
    else
    {
        strcpy(out, parent);
    }
    if (!debracket)
    {
        strcat(out, leaf);
    }
    else
    {
        int c = strlen(out) - 1;
        for (int i = 1; i < len - 1; ++i)
            out[c + i] = leaf[i];
        out[c + len - 1] = '\0';
    }
    return true;
}


// Given a file name, return the extension
bool path_get_ext(const char* file, char* ext)
{
    int len = strlen(file);
    if (len <= 0) return false;
    char* dot = strrchr(file, '.');
    if ((dot == 0) || (*(dot + 1) == '\0'))
    {
        return false;
    }
    strcpy(ext, dot + 1);
    return true;
}
