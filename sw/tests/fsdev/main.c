#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include "hw_conf.h"
#include "sw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "tick.h"
#include "disk.h"
#include "path_utils.h"
#include "lister.h"
#include "crc.h"    // Reuse from sdcard module

static char cur_dir[FF_LFN_BUF] = {0};

static void run_help();
static void run_ls();
static void run_cd();
static void run_cat();
static void run_rm();
static void run_lister();
static FRESULT cd(const char* dir);
static FRESULT ls(const char* dir);
static FRESULT cat(const char* dir);
static FRESULT rm(const char* dir);

typedef void (*p_fn_t)();
typedef struct 
{
    char const *const command;
    p_fn_t const function;
    char const *const help;
} cmd_def_t;



static cmd_def_t cmds[] = 
{
    {"ls", run_ls, "ls:\n ls [dir]"},
    {"cd", run_cd, "cd:\n cd <dir>"},
    {"cat", run_cat, "cat:\n cat <file>"},
    {"rm", run_rm, "rm:\n rm <file"},
    {"lister", run_lister, "lister:\n lister <dir>"},
    {"help", run_help, "help:\n  Shows this help."},
};


static void run_help() 
{
    for (size_t i = 0; i < count_of(cmds); ++i) 
    {
        printf("%s\n", cmds[i].help);
    }
}


static FRESULT cd(const char* dir)
{
    FRESULT fr = f_chdir(dir);
    if (FR_OK != fr)
    {
        // probably disk is not mount, try mount
        fr = disk_mount();
        if (FR_OK == fr)
        {
            fr = f_chdir(dir);
        }
    }
    if (FR_OK == fr)
    {
        path_copy(dir, cur_dir, FF_LFN_BUF);
    }
    return fr;
}


static FRESULT cat(const char* path)
{
    FRESULT fr;
    char buf[256];
    FIL fd;
    fr = f_open(&fd, path, FA_READ);
    if (FR_OK != fr)
    {
        printf("ls error: %s (%d)\n", disk_result_str(fr), fr);
        return fr;
    }
    while (f_gets(buf, 256, &fd))
    {
        printf("%s", buf);
    }
    f_close(&fd);
    return FR_OK;
}


static FRESULT rm(const char* path)
{
    FRESULT fr = f_unlink(path);
    if (FR_OK != fr)
    {
        printf("unlink error: %s (%d)\n", disk_result_str(fr), fr);
    }
    return fr;
}


FRESULT ls(const char* dir)
{
    FRESULT fr;
    
    DIR dj;      /* Directory object */
    FILINFO fno; /* File information */
    memset(&dj, 0, sizeof dj);
    memset(&fno, 0, sizeof fno);
    fr = f_findfirst(&dj, &fno, dir, "*");
    if (FR_OK != fr) 
    {
        return fr;
    }
    while (fr == FR_OK && fno.fname[0])  /* Repeat while an item is found */
    {
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        const char *pcWritableFile = "writable file",
                   *pcReadOnlyFile = "read only file",
                   *pcDirectory = "directory";
        const char *pcAttrib;
        /* Point pcAttrib to a string that describes the file. */
        if (fno.fattrib & AM_DIR) 
        {
            pcAttrib = pcDirectory;
        }
        else if (fno.fattrib & AM_RDO) 
        {
            pcAttrib = pcReadOnlyFile;
        }
        else
        {
            pcAttrib = pcWritableFile;
        }
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        printf("%s [%s] [size=%llu]\n", fno.fname, pcAttrib, fno.fsize);

        fr = f_findnext(&dj, &fno); /* Search for next item */
    }
    f_closedir(&dj);
    return fr;
}


static void run_ls() 
{
    FRESULT fr;
    char *arg1 = strtok(NULL, " ");
    absolute_time_t xStart = get_absolute_time();
    if (arg1) 
    {
        printf("Directory Listing: %s\n", arg1);
        fr = ls(arg1);
        if (FR_OK != fr)
        {
            printf("ls error: %s (%d)\n", disk_result_str(fr), fr);
            return;
        }
    }
    else
    {
        printf("Directory Listing: %s\n", cur_dir);
        fr = ls(cur_dir);
        if (FR_OK != fr)
        {
            printf("ls error: %s (%d)\n", disk_result_str(fr), fr);
            return;
        }
    }
    int64_t elapsed_us = absolute_time_diff_us(xStart, get_absolute_time());
    printf("Elapsed %0.1fms\n", elapsed_us / 1000.0f);
}


static void run_cat()
{
    char *arg1 = strtok(NULL, " ");
    if (!arg1) 
    {
        printf("Missing argument\n");
        return;
    }
    cat(arg1);
}


static void run_rm()
{
    char *arg1 = strtok(NULL, " ");
    if (!arg1) 
    {
        printf("Missing argument\n");
        return;
    }
    rm(arg1);
}


static void run_cd() 
{
    char *arg1 = strtok(NULL, " ");
    if (!arg1) 
    {
        printf("Missing argument\n");
        return;
    }
    FRESULT fr = cd(arg1);
    if (FR_OK != fr)
    {
        printf("f_chdir error: %s (%d)\n", disk_result_str(fr), fr);
    } 
}


static void run_lister() 
{
    int r;
    char path[FF_LFN_BUF + 1];
    char *arg1 = strtok(NULL, " ");
    const char *patterns[] = {
        "nsf", 0
    };
    absolute_time_t xStart = get_absolute_time();
    if (arg1) 
    {
        if (arg1[0] != '/')
        {
            path_concatenate("/", arg1, path, FF_LFN_BUF + 1, false);
        }
        else
        {
            path_copy(arg1, path, FF_LFN_BUF + 1);
        }
    }
    else
    {
        path_copy(cur_dir, path, FF_LFN_BUF + 1);
    }
    lister_t* lister = 0;
    r = lister_open_dir(path, 0, 20, true, &lister);
    if (LS_OK == r)
    {
        printf("Directory: %s, count=%d\n", lister->dir, lister->count);
        for (int page = 0; page < lister->pages; ++page)
        {
            lister_select_page(lister, page);
            uint8_t type;
            for (int i = 0; i < lister->page_size; ++i)
            {
                if (LS_OK == lister_get_entry(lister, i, path, FF_LFN_BUF + 1, &type))
                {
                    if (type == LS_TYPE_DIRECTORY)
                    {
                        printf("%02d:%03d: [%s]\n", page, i, path);
                    }
                    else
                    {
                        printf("%02d:%03d: %s\n", page, i, path);
                    }
                }
            }
        }
        lister_close(lister);
        int64_t elapsed_us = absolute_time_diff_us(xStart, get_absolute_time());
        printf("Elapsed %0.1fms\n", elapsed_us / 1000.0f);
    }
    else
    {
        printf("lister_open_dir failed with %d\n", r);
        if (r == LS_ERR_FATFS)
        {
            FRESULT fr = lister_get_fatfs_error();
            printf("FATFS error: %s (%d)\n", disk_result_str(fr), fr);
        }
    }
}


static void process_stdio(int ch)
{
    static char cmd[256];
    static size_t ix;

    if ((!isprint(ch))
        &&
        (!isspace(ch))
        &&
        ('\r' != ch)
        &&
        ('\b' != ch)
        &&
        (ch != (char)127)
       )
    { 
        return;
    }

    printf("%c", ch);  // echo
    stdio_flush();
    if (ch == '\r') 
    {
        /* Just to space the output from the input. */
        printf("\n");
        stdio_flush();

        if (!strnlen(cmd, sizeof cmd)) // Empty input
        {
            printf("> ");
            stdio_flush();
            return;
        }
        /* Process the input string received prior to the newline. */
        char *cmdn = strtok(cmd, " ");
        if (cmdn) 
        {
            size_t i;
            for (i = 0; i < count_of(cmds); ++i) 
            {
                if (0 == strcmp(cmds[i].command, cmdn)) 
                {
                    (*cmds[i].function)();
                    break;
                }
            }
            if (count_of(cmds) == i)
            {
                printf("Command \"%s\" not found\n", cmdn);
            }
        }
        ix = 0;
        memset(cmd, 0, sizeof cmd);
        printf("\n> ");
        stdio_flush();
    }
    else
    {  // Not newline
        if (ch == '\b' || ch == (char)127) 
        {
            /* Backspace was pressed.  Erase the last character in the string - if any. */
            if (ix > 0) 
            {
                ix--;
                cmd[ix] = '\0';
            }
        } 
        else
        {
            /* A character was entered.  Add it to the string
             entered so far.  When a \n is entered the complete
             string will be passed to the command interpreter. */
            if (ix < sizeof cmd - 1) 
            {
                cmd[ix] = ch;
                ix++;
            }
        }
    }
}



int main()
{
    uint32_t now;

    // main clock, calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);
    // tick timer
    tick_init();
    // init console
    stdio_init_all();
    printf("\033[2J\033[H"); // clear terminal
    stdio_flush();
    // in case using memory debugger
    MY_MEM_INIT();
    // initialize disk system
    disk_init();
    path_set_root(cur_dir);

    // loop timing and control flags
    uint32_t last_update_tick = tick_millis();

    // Super Loop
    for (;;)
    {
        int ret;
        now = tick_millis();
        if (now - last_update_tick >= SUPERLOOP_UPDATE_INTERVAL_MS)
        {
            // Disk update
            ret = disk_update(now);
            last_update_tick = now;
        }
        int ch = getchar_timeout_us(1000);
        // Get the character from terminal
        if (PICO_ERROR_TIMEOUT != ch) 
        {
            process_stdio(ch);
        }
    }
    
    return 0;
}
