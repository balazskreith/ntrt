/*********************************************************************
This file is part of the argtable2 library.
Copyright (C) 1998-2001,2003-2011 Stewart Heitmann
sheitmann@users.sourceforge.net

The argtable2 library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
**********************************************************************/
#include "etc_argtable2.h"

/* config.h must be included before anything else */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "./getopt.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


//#include "./getopt.h"

static
void arg_register_error(struct arg_end *end, void *parent, int error, const char *argval)
    {
    /* printf("arg_register_error(%p,%p,%d,%s)\n",end,parent,error,argval); */
    if (end->count < end->hdr.maxcount)
        {
        end->error[end->count] = error;
        end->parent[end->count] = parent;
        end->argval[end->count] = argval;
        end->count++;
        }
    else
        {
        end->error[end->hdr.maxcount-1]  = ARG_ELIMIT;
        end->parent[end->hdr.maxcount-1] = end;
        end->argval[end->hdr.maxcount-1] = NULL;
        }
    }


/*
 * Return index of first table entry with a matching short option
 * or -1 if no match was found.
 */
static
int find_shortoption(struct arg_hdr **table, char shortopt)
    {
    int tabindex;
    for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        if (table[tabindex]->shortopts && strchr(table[tabindex]->shortopts,shortopt))
            return tabindex;
        }
    return -1;
    }


struct longoptions
    {
    int getoptval;
    int noptions;
    struct option *options;
    };

#ifndef NDEBUG
static
void dump_longoptions(struct longoptions* longoptions)
    {
    int i;
    printf("getoptval = %d\n", longoptions->getoptval);
    printf("noptions  = %d\n", longoptions->noptions);
    for (i=0; i<longoptions->noptions; i++)
        {
        printf("options[%d].name    = \"%s\"\n", i, longoptions->options[i].name);
        printf("options[%d].has_arg = %d\n", i, longoptions->options[i].has_arg);
        printf("options[%d].flag    = %p\n", i, longoptions->options[i].flag);
        printf("options[%d].val     = %d\n", i, longoptions->options[i].val);
        }
    }
#endif

static
struct longoptions* alloc_longoptions(struct arg_hdr **table)
    {
    struct longoptions *result;
    size_t nbytes;
    int noptions = 1;
    size_t longoptlen = 0;
    int tabindex;

    /*
     * Determine the total number of option structs required
     * by counting the number of comma separated long options
     * in all table entries and return the count in noptions.
     * note: noptions starts at 1 not 0 because we getoptlong
     * requires a NULL option entry to terminate the option array.
     * While we are at it, count the number of chars required
     * to store private copies of all the longoption strings
     * and return that count in logoptlen.
     */
     tabindex=0;
     do
        {
        const char *longopts = table[tabindex]->longopts;
        longoptlen += (longopts?strlen(longopts):0) + 1;
        while (longopts)
            {
            noptions++;
            longopts = strchr(longopts+1,',');
            }
        }while(!(table[tabindex++]->flag&ARG_TERMINATOR));
    /*printf("%d long options consuming %d chars in total\n",noptions,longoptlen);*/


    /* allocate storage for return data structure as: */
    /* (struct longoptions) + (struct options)[noptions] + char[longoptlen] */
    nbytes = sizeof(struct longoptions)
           + sizeof(struct option)*noptions
           + longoptlen;
    result = (struct longoptions*)malloc(nbytes);
    if (result)
        {
        int option_index=0;
        char *store;

        result->getoptval=0;
        result->noptions = noptions;
        result->options = (struct option*)(result + 1);
        store = (char*)(result->options + noptions);

        for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
            {
            const char *longopts = table[tabindex]->longopts;

            while(longopts && *longopts)
                {
                char *storestart = store;

                /* copy progressive longopt strings into the store */
                while (*longopts!=0 && *longopts!=',')
                    *store++ = *longopts++;
                *store++ = 0;
                if (*longopts==',')
                    longopts++;
                /*fprintf(stderr,"storestart=\"%s\"\n",storestart);*/

                result->options[option_index].name    = storestart;
                result->options[option_index].flag    = &(result->getoptval);
                result->options[option_index].val     = tabindex;
                if (table[tabindex]->flag & ARG_HASOPTVALUE)
                    result->options[option_index].has_arg = 2;
                else if (table[tabindex]->flag & ARG_HASVALUE)
                    result->options[option_index].has_arg = 1;
                else
                    result->options[option_index].has_arg = 0;

                option_index++;
                }
            }
        /* terminate the options array with a zero-filled entry */
        result->options[option_index].name    = 0;
        result->options[option_index].has_arg = 0;
        result->options[option_index].flag    = 0;
        result->options[option_index].val     = 0;
        }

    /*dump_longoptions(result);*/
    return result;
    }

static
char* alloc_shortoptions(struct arg_hdr **table)
   {
   char *result;
   size_t len = 2;
   int tabindex;

   /* determine the total number of option chars required */
   for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
       {
       struct arg_hdr *hdr = table[tabindex];
       len += 3 * (hdr->shortopts?strlen(hdr->shortopts):0);
       }

   result = malloc(len);
   if (result)
        {
        char *res = result;

        /* add a leading ':' so getopt return codes distinguish    */
        /* unrecognised option and options missing argument values */
        *res++=':';

        for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
            {
            struct arg_hdr *hdr = table[tabindex];
            const char *shortopts = hdr->shortopts;
            while(shortopts && *shortopts)
                {
                *res++ = *shortopts++;
                if (hdr->flag & ARG_HASVALUE)
                    *res++=':';
                if (hdr->flag & ARG_HASOPTVALUE)
                    *res++=':';
                }
            }
        /* null terminate the string */
        *res=0;
        }

   /*printf("alloc_shortoptions() returns \"%s\"\n",(result?result:"NULL"));*/
   return result;
   }


/* return index of the table terminator entry */
static
int arg_endindex(struct arg_hdr **table)
    {
    int tabindex=0;
    while (!(table[tabindex]->flag&ARG_TERMINATOR))
        tabindex++;
    return tabindex;
    }


static
void arg_parse_tagged(int argc, char **argv, struct arg_hdr **table, struct arg_end *endtable)
    {
    struct longoptions *longoptions;
    char *shortoptions;
    int copt;

    /*printf("arg_parse_tagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/

    /* allocate short and long option arrays for the given opttable[].   */
    /* if the allocs fail then put an error msg in the last table entry. */
    longoptions  = alloc_longoptions(table);
    shortoptions = alloc_shortoptions(table);
    if (!longoptions || !shortoptions)
        {
        /* one or both memory allocs failed */
        arg_register_error(endtable,endtable,ARG_EMALLOC,NULL);
        /* free anything that was allocated (this is null safe) */
        free(shortoptions);
        free(longoptions);
        return;
        }

    /*dump_longoptions(longoptions);*/

    /* reset getopts internal option-index to zero, and disable error reporting */
    optind = 0;
    opterr = 0;

    /* fetch and process args using getopt_long */
    while( (copt=getopt_long(argc,argv,shortoptions,longoptions->options,NULL)) != -1)
        {
        /*
        printf("optarg='%s'\n",optarg);
        printf("optind=%d\n",optind);
        printf("copt=%c\n",(char)copt);
        printf("optopt=%c (%d)\n",optopt, (int)(optopt));
        */
        switch(copt)
            {
            case 0:
                {
                int tabindex = longoptions->getoptval;
                void *parent  = table[tabindex]->parent;
                /*printf("long option detected from argtable[%d]\n", tabindex);*/
                if (optarg && optarg[0]==0 && (table[tabindex]->flag & ARG_HASVALUE))
                    {
                    /* printf(": long option %s requires an argument\n",argv[optind-1]); */
                    arg_register_error(endtable,endtable,ARG_EMISSARG,argv[optind-1]);
                    /* continue to scan the (empty) argument value to enforce argument count checking */
                    }
                if (table[tabindex]->scanfn)
                    {
                    int errorcode = table[tabindex]->scanfn(parent,optarg);
                    if (errorcode!=0)
                        arg_register_error(endtable,parent,errorcode,optarg);
                    }
                }
                break;

            case '?':
                /*
                * getopt_long() found an unrecognised short option.
                * if it was a short option its value is in optopt
                * if it was a long option then optopt=0
                */
                switch (optopt)
                    {
                    case 0:
                        /*printf("?0 unrecognised long option %s\n",argv[optind-1]);*/
                        arg_register_error(endtable,endtable,ARG_ELONGOPT,argv[optind-1]);
                        break;
                    default:
                        /*printf("?* unrecognised short option '%c'\n",optopt);*/
                        arg_register_error(endtable,endtable,optopt,NULL);
                        break;
                    }
                break;

            case':':
                /*
                * getopt_long() found an option with its argument missing.
                */
                /*printf(": option %s requires an argument\n",argv[optind-1]); */
                arg_register_error(endtable,endtable,ARG_EMISSARG,argv[optind-1]);
                break;

            default:
                {
                /* getopt_long() found a valid short option */
                int tabindex = find_shortoption(table,(char)copt);
                /*printf("short option detected from argtable[%d]\n", tabindex);*/
                if (tabindex==-1)
                    {
                    /* should never get here - but handle it just in case */
                    /*printf("unrecognised short option %d\n",copt);*/
                    arg_register_error(endtable,endtable,copt,NULL);
                    }
                else
                    {
                    if (table[tabindex]->scanfn)
                        {
                        void *parent  = table[tabindex]->parent;
                        int errorcode = table[tabindex]->scanfn(parent,optarg);
                        if (errorcode!=0)
                            arg_register_error(endtable,parent,errorcode,optarg);
                        }
                    }
                break;
                }
            }
        }

    free(shortoptions);
    free(longoptions);
    }


static
void arg_parse_untagged(int argc, char **argv, struct arg_hdr **table, struct arg_end *endtable)
    {
    int tabindex=0;
    int errorlast=0;
    const char *optarglast = NULL;
    void *parentlast = NULL;

    /*printf("arg_parse_untagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/
    while (!(table[tabindex]->flag&ARG_TERMINATOR))
        {
        void *parent;
        int errorcode;

        /* if we have exhausted our argv[optind] entries then we have finished */
        if (optind>=argc)
            {
            /*printf("arg_parse_untagged(): argv[] exhausted\n");*/
            return;
            }

        /* skip table entries with non-null long or short options (they are not untagged entries) */
        if (table[tabindex]->longopts || table[tabindex]->shortopts)
            {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (tagged argument)\n",tabindex);*/
            tabindex++;
            continue;
            }

        /* skip table entries with NULL scanfn */
        if (!(table[tabindex]->scanfn))
            {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (NULL scanfn)\n",tabindex);*/
            tabindex++;
            continue;
            }

        /* attempt to scan the current argv[optind] with the current     */
        /* table[tabindex] entry. If it succeeds then keep it, otherwise */
        /* try again with the next table[] entry.                        */
        parent = table[tabindex]->parent;
        errorcode = table[tabindex]->scanfn(parent,argv[optind]);
        if (errorcode==0)
            {
            /* success, move onto next argv[optind] but stay with same table[tabindex] */
            /*printf("arg_parse_untagged(): argtable[%d] successfully matched\n",tabindex);*/
            optind++;

            /* clear the last tentative error */
            errorlast = 0;
            }
        else
            {
            /* failure, try same argv[optind] with next table[tabindex] entry */
            /*printf("arg_parse_untagged(): argtable[%d] failed match\n",tabindex);*/
            tabindex++;

            /* remember this as a tentative error we may wish to reinstate later */
            errorlast = errorcode;
            optarglast = argv[optind];
            parentlast = parent;
            }

        }

    /* if a tenative error still remains at this point then register it as a proper error */
    if (errorlast)
        {
        arg_register_error(endtable,parentlast,errorlast,optarglast);
        optind++;
        }

    /* only get here when not all argv[] entries were consumed */
    /* register an error for each unused argv[] entry */
    while (optind<argc)
        {
        /*printf("arg_parse_untagged(): argv[%d]=\"%s\" not consumed\n",optind,argv[optind]);*/
        arg_register_error(endtable,endtable,ARG_ENOMATCH,argv[optind++]);
        }

    return;
    }


static
void arg_parse_check(struct arg_hdr **table, struct arg_end *endtable)
    {
    int tabindex=0;
    /* printf("arg_parse_check()\n"); */
    do
        {
        if (table[tabindex]->checkfn)
            {
            void *parent  = table[tabindex]->parent;
            int errorcode = table[tabindex]->checkfn(parent);
            if (errorcode!=0)
                arg_register_error(endtable,parent,errorcode,NULL);
            }
        }while(!(table[tabindex++]->flag&ARG_TERMINATOR));
    }


static
void arg_reset(void **argtable)
    {
    struct arg_hdr **table=(struct arg_hdr**)argtable;
    int tabindex=0;
    /*printf("arg_reset(%p)\n",argtable);*/
    do
        {
        if (table[tabindex]->resetfn)
            table[tabindex]->resetfn(table[tabindex]->parent);
        } while(!(table[tabindex++]->flag&ARG_TERMINATOR));
    }


int arg_parse(int argc, char **argv, void **argtable)
    {
    struct arg_hdr **table = (struct arg_hdr **)argtable;
    struct arg_end *endtable;
    int endindex;
    char **argvcopy = NULL;

    /*printf("arg_parse(%d,%p,%p)\n",argc,argv,argtable);*/

    /* reset any argtable data from previous invocations */
    arg_reset(argtable);

    /* locate the first end-of-table marker within the array */
    endindex = arg_endindex(table);
    endtable = (struct arg_end*)table[endindex];

    /* Special case of argc==0.  This can occur on Texas Instruments DSP. */
    /* Failure to trap this case results in an unwanted NULL result from  */
    /* the malloc for argvcopy (next code block).                         */
    if (argc==0)
        {
        /* We must still perform post-parse checks despite the absence of command line arguments */
        arg_parse_check(table,endtable);

        /* Now we are finished */
        return endtable->count;
        }

    argvcopy = malloc(sizeof(char *) * argc);
    if (argvcopy)
        {
        int i;

        /*
        Fill in the local copy of argv[]. We need a local copy
        because getopt rearranges argv[] which adversely affects
        susbsequent parsing attempts.
        */
        for (i=0; i<argc; i++)
            argvcopy[i] = argv[i];

        /* parse the command line (local copy) for tagged options */
        arg_parse_tagged(argc,argvcopy,table,endtable);

        /* parse the command line (local copy) for untagged options */
        arg_parse_untagged(argc,argvcopy,table,endtable);

        /* if no errors so far then perform post-parse checks otherwise dont bother */
        if (endtable->count==0)
            arg_parse_check(table,endtable);

        /* release the local copt of argv[] */
        free(argvcopy);
        }
    else
        {
        /* memory alloc failed */
        arg_register_error(endtable,endtable,ARG_EMALLOC,NULL);
        }

    return endtable->count;
    }


/*
 * Concatenate contents of src[] string onto *pdest[] string.
 * The *pdest pointer is altered to point to the end of the
 * target string and *pndest is decremented by the same number
 * of chars.
 * Does not append more than *pndest chars into *pdest[]
 * so as to prevent buffer overruns.
 * Its something like strncat() but more efficient for repeated
 * calls on the same destination string.
 * Example of use:
 *   char dest[30] = "good"
 *   size_t ndest = sizeof(dest);
 *   char *pdest = dest;
 *   arg_char(&pdest,"bye ",&ndest);
 *   arg_char(&pdest,"cruel ",&ndest);
 *   arg_char(&pdest,"world!",&ndest);
 * Results in:
 *   dest[] == "goodbye cruel world!"
 *   ndest  == 10
 */
static
void arg_cat(char **pdest, const char *src, size_t *pndest)
    {
    char *dest = *pdest;
    char *end  = dest + *pndest;

    /*locate null terminator of dest string */
    while(dest<end && *dest!=0)
        dest++;

    /* concat src string to dest string */
    while(dest<end && *src!=0)
        *dest++ = *src++;

    /* null terminate dest string */
    *dest=0;

    /* update *pdest and *pndest */
    *pndest = end - dest;
    *pdest  = dest;
    }


static
void arg_cat_option(char *dest, size_t ndest, const char *shortopts, const char *longopts, const char *datatype, int optvalue)
    {
    if (shortopts)
        {
        char option[3];

        /* note: option array[] is initialiazed dynamically here to satisfy   */
        /* a deficiency in the watcom compiler wrt static array initializers. */
        option[0] = '-';
        option[1] = shortopts[0];
        option[2] = 0;

        arg_cat(&dest,option,&ndest);
        if (datatype)
            {
            arg_cat(&dest," ",&ndest);
            if (optvalue)
                {
                arg_cat(&dest,"[",&ndest);
                arg_cat(&dest,datatype,&ndest);
                arg_cat(&dest,"]",&ndest);
                }
            else
                arg_cat(&dest,datatype,&ndest);
            }
        }
    else if (longopts)
        {
        size_t ncspn;

        /* add "--" tag prefix */
        arg_cat(&dest,"--",&ndest);

        /* add comma separated option tag */
        ncspn = strcspn(longopts,",");
        strncat(dest,longopts,(ncspn<ndest)?ncspn:ndest);

        if (datatype)
            {
            arg_cat(&dest,"=",&ndest);
            if (optvalue)
                {
                arg_cat(&dest,"[",&ndest);
                arg_cat(&dest,datatype,&ndest);
                arg_cat(&dest,"]",&ndest);
                }
            else
                arg_cat(&dest,datatype,&ndest);
            }
        }
    else if (datatype)
        {
        if (optvalue)
            {
            arg_cat(&dest,"[",&ndest);
            arg_cat(&dest,datatype,&ndest);
            arg_cat(&dest,"]",&ndest);
            }
        else
            arg_cat(&dest,datatype,&ndest);
        }
    }

static
void arg_cat_optionv(char *dest, size_t ndest, const char *shortopts, const char *longopts, const char *datatype,  int optvalue, const char *separator)
    {
    separator = separator ? separator : "";

    if (shortopts)
        {
        const char *c = shortopts;
        while(*c)
            {
            /* "-a|-b|-c" */
            char shortopt[3];

            /* note: shortopt array[] is initialiazed dynamically here to satisfy */
            /* a deficiency in the watcom compiler wrt static array initializers. */
            shortopt[0]='-';
            shortopt[1]=*c;
            shortopt[2]=0;

            arg_cat(&dest,shortopt,&ndest);
            if (*++c)
                arg_cat(&dest,separator,&ndest);
            }
        }

    /* put separator between long opts and short opts */
    if (shortopts && longopts)
        arg_cat(&dest,separator,&ndest);

    if (longopts)
        {
        const char *c = longopts;
        while(*c)
            {
            size_t ncspn;

            /* add "--" tag prefix */
            arg_cat(&dest,"--",&ndest);

            /* add comma separated option tag */
            ncspn = strcspn(c,",");
            strncat(dest,c,(ncspn<ndest)?ncspn:ndest);
            c+=ncspn;

            /* add given separator in place of comma */
            if (*c==',')
                 {
                 arg_cat(&dest,separator,&ndest);
                 c++;
                 }
            }
        }

    if (datatype)
        {
        if (longopts)
            arg_cat(&dest,"=",&ndest);
        else if (shortopts)
            arg_cat(&dest," ",&ndest);

        if (optvalue)
            {
            arg_cat(&dest,"[",&ndest);
            arg_cat(&dest,datatype,&ndest);
            arg_cat(&dest,"]",&ndest);
            }
        else
            arg_cat(&dest,datatype,&ndest);
        }
    }


/* this function should be deprecated because it doesnt consider optional argument values (ARG_HASOPTVALUE) */
void arg_print_option(FILE *fp, const char *shortopts, const char *longopts, const char *datatype, const char *suffix)
    {
    char syntax[200]="";
    suffix = suffix ? suffix : "";

    /* there is no way of passing the proper optvalue for optional argument values here, so we must ignore it */
    arg_cat_optionv(syntax,sizeof(syntax),shortopts,longopts,datatype,0,"|");

    fputs(syntax,fp);
    fputs(suffix,fp);
    }


/*
 * Print a GNU style [OPTION] string in which all short options that
 * do not take argument values are presented in abbreviated form, as
 * in: -xvfsd, or -xvf[sd], or [-xvsfd]
 */
static
void arg_print_gnuswitch(FILE *fp, struct arg_hdr **table)
    {
    int tabindex;
    char *format1=" -%c";
    char *format2=" [-%c";
    char *suffix="";

    /* print all mandatory switches that are without argument values */
    for(tabindex=0; table[tabindex] && !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        /* skip optional options */
        if (table[tabindex]->mincount<1)
            continue;

        /* skip non-short options */
        if (table[tabindex]->shortopts==NULL)
            continue;

        /* skip options that take argument values */
        if (table[tabindex]->flag&ARG_HASVALUE)
            continue;

        /* print the short option (only the first short option char, ignore multiple choices)*/
        fprintf(fp,format1,table[tabindex]->shortopts[0]);
        format1="%c";
        format2="[%c";
        }

    /* print all optional switches that are without argument values */
    for(tabindex=0; table[tabindex] && !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        /* skip mandatory args */
        if (table[tabindex]->mincount>0)
            continue;

        /* skip args without short options */
        if (table[tabindex]->shortopts==NULL)
            continue;

        /* skip args with values */
        if (table[tabindex]->flag&ARG_HASVALUE)
            continue;

        /* print first short option */
        fprintf(fp,format2,table[tabindex]->shortopts[0]);
        format2="%c";
        suffix="]";
        }

    fprintf(fp,"%s",suffix);
    }


void arg_print_syntax(FILE *fp, void **argtable, const char *suffix)
    {
    struct arg_hdr **table = (struct arg_hdr**)argtable;
    int i,tabindex;

    /* print GNU style [OPTION] string */
    arg_print_gnuswitch(fp, table);

    /* print remaining options in abbreviated style */
    for(tabindex=0; table[tabindex] && !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        char syntax[200]="";
        const char *shortopts, *longopts, *datatype;

        /* skip short options without arg values (they were printed by arg_print_gnu_switch) */
        if (table[tabindex]->shortopts && !(table[tabindex]->flag&ARG_HASVALUE))
            continue;

        shortopts = table[tabindex]->shortopts;
        longopts  = table[tabindex]->longopts;
        datatype  = table[tabindex]->datatype;
        arg_cat_option(syntax,sizeof(syntax),shortopts,longopts,datatype, table[tabindex]->flag&ARG_HASOPTVALUE);

        if (strlen(syntax)>0)
            {
            /* print mandatory instances of this option */
            for (i=0; i<table[tabindex]->mincount; i++)
                fprintf(fp, " %s",syntax);

            /* print optional instances enclosed in "[..]" */
            switch ( table[tabindex]->maxcount - table[tabindex]->mincount )
                {
                case 0:
                    break;
                case 1:
                    fprintf(fp, " [%s]",syntax);
                    break;
                case 2:
                    fprintf(fp, " [%s] [%s]",syntax,syntax);
                    break;
                default:
                    fprintf(fp, " [%s]...",syntax);
                    break;
                }
            }
        }

    if (suffix)
        fprintf(fp, "%s",suffix);
    }


void arg_print_syntaxv(FILE *fp, void **argtable, const char *suffix)
    {
    struct arg_hdr **table = (struct arg_hdr**)argtable;
    int i,tabindex;

    /* print remaining options in abbreviated style */
    for(tabindex=0; table[tabindex] && !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        char syntax[200]="";
        const char *shortopts, *longopts, *datatype;

        shortopts = table[tabindex]->shortopts;
        longopts  = table[tabindex]->longopts;
        datatype  = table[tabindex]->datatype;
        arg_cat_optionv(syntax,sizeof(syntax),shortopts,longopts,datatype,table[tabindex]->flag&ARG_HASOPTVALUE, "|");

        /* print mandatory options */
        for (i=0; i<table[tabindex]->mincount; i++)
            fprintf(fp," %s",syntax);

        /* print optional args enclosed in "[..]" */
        switch ( table[tabindex]->maxcount - table[tabindex]->mincount )
            {
            case 0:
                break;
            case 1:
                fprintf(fp, " [%s]",syntax);
                break;
            case 2:
                fprintf(fp, " [%s] [%s]",syntax,syntax);
                break;
            default:
                fprintf(fp, " [%s]...",syntax);
                break;
            }
        }

    if (suffix)
        fprintf(fp,"%s",suffix);
    }


void arg_print_glossary(FILE *fp, void **argtable, const char *format)
    {
    struct arg_hdr **table = (struct arg_hdr**)argtable;
    int tabindex;

    format = format ? format : "  %-20s %s\n";
    for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        if (table[tabindex]->glossary)
            {
            char syntax[200]="";
            const char *shortopts = table[tabindex]->shortopts;
            const char *longopts  = table[tabindex]->longopts;
            const char *datatype  = table[tabindex]->datatype;
            const char *glossary  = table[tabindex]->glossary;
            arg_cat_optionv(syntax,sizeof(syntax),shortopts,longopts,datatype,table[tabindex]->flag&ARG_HASOPTVALUE,", ");
            fprintf(fp,format,syntax,glossary);
            }
        }
    }


/**
 * Print a piece of text formatted, which means in a column with a
 * left and a right margin. The lines are wrapped at whitspaces next
 * to right margin. The function does not indent the first line, but
 * only the following ones.
 *
 * Example:
 * arg_print_formatted( fp, 0, 5, "Some text that doesn't fit." )
 * will result in the following output:
 *
 * Some
 * text
 * that
 * doesn'
 * t fit.
 *
 * Too long lines will be wrapped in the middle of a word.
 *
 * arg_print_formatted( fp, 2, 7, "Some text that doesn't fit." )
 * will result in the following output:
 *
 * Some
 *   text
 *   that
 *   doesn'
 *   t fit.
 *
 * As you see, the first line is not indented. This enables output of
 * lines, which start in a line where output already happened.
 *
 * Author: Uli Fouquet
 */
static
void arg_print_formatted( FILE *fp, const unsigned lmargin, const unsigned rmargin, const char *text )
    {
    const unsigned textlen = strlen( text );
    unsigned line_start = 0;
    unsigned line_end = textlen + 1;
    const unsigned colwidth = (rmargin - lmargin) + 1;

    /* Someone doesn't like us... */
    if ( line_end < line_start )
        { fprintf( fp, "%s\n", text ); }

    while (line_end-1 > line_start )
        {
        /* Eat leading whitespaces. This is essential because while
           wrapping lines, there will often be a whitespace at beginning
           of line  */
        while ( isspace(*(text+line_start)) )
            { line_start++; }

        if ((line_end - line_start) > colwidth )
            { line_end = line_start + colwidth; }

        /* Find last whitespace, that fits into line */
        while ( ( line_end > line_start )
                && ( line_end - line_start > colwidth )
                && !isspace(*(text+line_end)))
            { line_end--; }

        /* Do not print trailing whitespace. If this text
           has got only one line, line_end now points to the
           last char due to initialization. */
        line_end--;

        /* Output line of text */
        while ( line_start < line_end )
            {
            fputc(*(text+line_start), fp );
            line_start++;
            }
        fputc( '\n', fp );

        /* Initialize another line */
        if ( line_end+1 < textlen )
            {
            unsigned i;

            for (i=0; i < lmargin; i++ )
                { fputc( ' ', fp ); }

            line_end = textlen;
            }

        /* If we have to print another line, get also the last char. */
        line_end++;

        } /* lines of text */
    }

/**
 * Prints the glossary in strict GNU format.
 * Differences to arg_print_glossary() are:
 *  - wraps lines after 80 chars
 *  - indents lines without shortops
 *  - does not accept formatstrings
 *
 * Contributed by Uli Fouquet
 */
void arg_print_glossary_gnu(FILE *fp, void **argtable )
    {
    struct arg_hdr **table = (struct arg_hdr**)argtable;
    int tabindex;

    for(tabindex=0; !(table[tabindex]->flag&ARG_TERMINATOR); tabindex++)
        {
        if (table[tabindex]->glossary)
            {
            char syntax[200]="";
            const char *shortopts = table[tabindex]->shortopts;
            const char *longopts  = table[tabindex]->longopts;
            const char *datatype  = table[tabindex]->datatype;
            const char *glossary  = table[tabindex]->glossary;

            if ( !shortopts && longopts )
                {
                /* Indent trailing line by 4 spaces... */
                memset( syntax, ' ', 4 );
                *(syntax+4) = '\0';
                }

            arg_cat_optionv(syntax,sizeof(syntax),shortopts,longopts,datatype,table[tabindex]->flag&ARG_HASOPTVALUE,", ");

            /* If syntax fits not into column, print glossary in new line... */
            if ( strlen(syntax) > 25 )
                {
                fprintf( fp, "  %-25s %s\n", syntax, "" );
                *syntax = '\0';
                }

            fprintf( fp, "  %-25s ", syntax );
            arg_print_formatted( fp, 28, 79, glossary );
            }
        } /* for each table entry */

    fputc( '\n', fp );
    }


/**
 * Checks the argtable[] array for NULL entries and returns 1
 * if any are found, zero otherwise.
 */
int arg_nullcheck(void **argtable)
    {
    struct arg_hdr **table = (struct arg_hdr **)argtable;
    int tabindex;
    /*printf("arg_nullcheck(%p)\n",argtable);*/

    if (!table)
        return 1;

    tabindex=0;
    do
        {
        /*printf("argtable[%d]=%p\n",tabindex,argtable[tabindex]);*/
        if (!table[tabindex])
            return 1;
        } while(!(table[tabindex++]->flag&ARG_TERMINATOR));

    return 0;
    }


/*
 * arg_free() is deprecated in favour of arg_freetable() due to a flaw in its design.
 * The flaw results in memory leak in the (very rare) case that an intermediate
 * entry in the argtable array failed its memory allocation while others following
 * that entry were still allocated ok. Those subsequent allocations will not be
 * deallocated by arg_free().
 * Despite the unlikeliness of the problem occurring, and the even unlikelier event
 * that it has any deliterious effect, it is fixed regardless by replacing arg_free()
 * with the newer arg_freetable() function.
 * We still keep arg_free() for backwards compatibility.
 */
void arg_free(void **argtable)
    {
    struct arg_hdr **table=(struct arg_hdr**)argtable;
    int tabindex=0;
    int flag;
    /*printf("arg_free(%p)\n",argtable);*/
    do
        {
        /*
        if we encounter a NULL entry then somewhat incorrectly we presume
        we have come to the end of the array. It isnt strictly true because
        an intermediate entry could be NULL with other non-NULL entries to follow.
        The subsequent argtable entries would then not be freed as they should.
        */
        if (table[tabindex]==NULL)
            break;

        flag = table[tabindex]->flag;
        free(table[tabindex]);
        table[tabindex++]=NULL;

        } while(!(flag&ARG_TERMINATOR));
    }

/* frees each non-NULL element of argtable[], where n is the size of the number of entries in the array */
void arg_freetable(void **argtable, size_t n)
    {
    struct arg_hdr **table=(struct arg_hdr**)argtable;
    int tabindex=0;
    /*printf("arg_freetable(%p)\n",argtable);*/
    for (tabindex=0; tabindex<n; tabindex++)
        {
        if (table[tabindex]==NULL)
            continue;

        free(table[tabindex]);
        table[tabindex]=NULL;
        };
    }


//--------------------------------------------------------------------
//------------------------ arg_date.c -----------------------------
//--------------------------------------------------------------------

/* local error codes  */
enum {
	EMINCOUNT_DATE   = 1,
	EMAXCOUNT_DATE   = 2,
	EBADDATE         = 3,
	EMINCOUNT_DOUBLE = 5,
	EMAXCOUNT_DOUBLE = 6,
	EBADDOUBLE       = 7,
	EMINCOUNT_FILE   = 9,
	EMAXCOUNT_FILE   = 10,
	EMINCOUNT_INT    = 11,
	EMAXCOUNT_INT    = 12,
	EBADINT          = 13,
	EOVERFLOW_INT    = 14,
	EMINCOUNT_LIT    = 15,
	EMAXCOUNT_LIT    = 16,
	EMINCOUNT_REX    = 200,
	EMAXCOUNT_REX    = 201

};

static void resetfn_date(struct arg_date *parent)
{
	/*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
	parent->count=0;
}

static int scanfn_date(struct arg_date *parent, const char *argval)
{
int errorcode = 0;

if (parent->count == parent->hdr.maxcount )
	errorcode = EMAXCOUNT_DATE;
else if (!argval)
	{
	/* no argument value was given, leave parent->tmval[] unaltered but still count it */
	parent->count++;
	}
else
	{
	const char *pend;
	struct tm tm = parent->tmval[parent->count];

	/* parse the given argument value, store result in parent->tmval[] */
	pend = strptime(argval, parent->format, &tm);
	if (pend && pend[0]=='\0')
		parent->tmval[parent->count++] = tm;
	else
		errorcode = EBADDATE;
	}

/*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
return errorcode;
}

static int checkfn_date(struct arg_date *parent)
{
	int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_DATE : 0;

	/*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
	return errorcode;
}

static void errorfn_date(struct arg_date *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
{
	const char *shortopts = parent->hdr.shortopts;
	const char *longopts  = parent->hdr.longopts;
	const char *datatype  = parent->hdr.datatype;

	/* make argval NULL safe */
	argval = argval ? argval : "";

	fprintf(fp,"%s: ",progname);
	switch(errorcode)
	{
		case EMINCOUNT_DATE:
			fputs("missing option ",fp);
			arg_print_option(fp,shortopts,longopts,datatype,"\n");
			break;

		case EMAXCOUNT_DATE:
			fputs("excess option ",fp);
			arg_print_option(fp,shortopts,longopts,argval,"\n");
			break;

		case EBADDATE:
		{
			struct tm tm;
			char buff[200];

			fprintf(fp,"illegal timestamp format \"%s\"\n",argval);
			bzero(&tm,sizeof(tm));
			strptime("1999-12-31 23:59:59","%F %H:%M:%S",&tm);
			strftime(buff, sizeof(buff), parent->format, &tm);
			printf("correct format is \"%s\"\n", buff);
			break;
		}
	}
}


struct arg_date* arg_date0(const char* shortopts,
					   const char* longopts,
					   const char* format,
					   const char *datatype,
					   const char *glossary)
{
	return arg_daten(shortopts,longopts,format,datatype,0,1,glossary);
}

struct arg_date* arg_date1(const char* shortopts,
					   const char* longopts,
					   const char* format,
					   const char *datatype,
					   const char *glossary)
{
	return arg_daten(shortopts,longopts,format,datatype,1,1,glossary);
}


struct arg_date* arg_daten(const char* shortopts,
					   const char* longopts,
					   const char* format,
					   const char *datatype,
					   int mincount,
					   int maxcount,
					   const char *glossary)
{
	size_t nbytes;
	struct arg_date *result;

	/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

	/* default time format is the national date format for the locale */
	if (!format)
		format = "%x";

	nbytes = sizeof(struct arg_date)         /* storage for struct arg_date */
	   + maxcount*sizeof(struct tm);     /* storage for tmval[maxcount] array */

	/* allocate storage for the arg_date struct + tmval[] array.    */
	/* we use calloc because we want the tmval[] array zero filled. */
	result = (struct arg_date*)calloc(1,nbytes);
	if (result)
	{
		/* init the arg_hdr struct */
		result->hdr.flag      = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts  = longopts;
		result->hdr.datatype  = datatype ? datatype : format;
		result->hdr.glossary  = glossary;
		result->hdr.mincount  = mincount;
		result->hdr.maxcount  = maxcount;
		result->hdr.parent    = result;
		result->hdr.resetfn   = (arg_resetfn*)resetfn_date;
		result->hdr.scanfn    = (arg_scanfn*)scanfn_date;
		result->hdr.checkfn   = (arg_checkfn*)checkfn_date;
		result->hdr.errorfn   = (arg_errorfn*)errorfn_date;

		/* store the tmval[maxcount] array immediately after the arg_date struct */
		result->tmval  = (struct tm*)(result+1);

		/* init the remaining arg_date member variables */
		result->count = 0;
		result->format = format;
	}

	/*printf("arg_daten() returns %p\n",result);*/
	return result;
}

//--------------------------------------------------------------------
//------------------------ arg_dbl.c -----------------------------
//--------------------------------------------------------------------

static void resetfn_double(struct arg_dbl *parent)
{
/*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
parent->count=0;
}

static int scanfn_double(struct arg_dbl *parent, const char *argval)
{
int errorcode = 0;

if (parent->count == parent->hdr.maxcount)
	{
	/* maximum number of arguments exceeded */
	errorcode = EMAXCOUNT_DOUBLE;
	}
else if (!argval)
	{
	/* a valid argument with no argument value was given. */
	/* This happens when an optional argument value was invoked. */
	/* leave parent arguiment value unaltered but still count the argument. */
	parent->count++;
	}
else
	{
	double val;
	char *end;

	/* extract double from argval into val */
	val = strtod(argval,&end);

	/* if success then store result in parent->dval[] array otherwise return error*/
	if (*end==0)
		parent->dval[parent->count++] = val;
	else
		errorcode = EBADDOUBLE;
	}

/*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
return errorcode;
}

static int checkfn_double(struct arg_dbl *parent)
{
int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_DOUBLE : 0;
/*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
return errorcode;
}

static void errorfn_double(struct arg_dbl *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
{
const char *shortopts = parent->hdr.shortopts;
const char *longopts  = parent->hdr.longopts;
const char *datatype  = parent->hdr.datatype;

/* make argval NULL safe */
argval = argval ? argval : "";

fprintf(fp,"%s: ",progname);
switch(errorcode)
	{
	case EMINCOUNT_DOUBLE:
		fputs("missing option ",fp);
		arg_print_option(fp,shortopts,longopts,datatype,"\n");
		break;

	case EMAXCOUNT_DOUBLE:
		fputs("excess option ",fp);
		arg_print_option(fp,shortopts,longopts,argval,"\n");
		break;

	case EBADDOUBLE:
		fprintf(fp,"invalid argument \"%s\" to option ",argval);
		arg_print_option(fp,shortopts,longopts,datatype,"\n");
		break;
	}
}


struct arg_dbl* arg_dbl0(const char* shortopts,
						   const char* longopts,
						   const char *datatype,
						   const char *glossary)
{
	return arg_dbln(shortopts,longopts,datatype,0,1,glossary);
}

struct arg_dbl* arg_dbl1(const char* shortopts,
						   const char* longopts,
						   const char *datatype,
						   const char *glossary)
{
	return arg_dbln(shortopts,longopts,datatype,1,1,glossary);
}


struct arg_dbl* arg_dbln(const char* shortopts,
						   const char* longopts,
						   const char *datatype,
						   int mincount,
						   int maxcount,
						   const char *glossary)
{
	size_t nbytes;
	struct arg_dbl *result;

/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

	nbytes = sizeof(struct arg_dbl)     /* storage for struct arg_dbl */
	   + (maxcount+1) * sizeof(double);  /* storage for dval[maxcount] array plus one extra for padding to memory boundary */

	result = (struct arg_dbl*)malloc(nbytes);
	if (result)
	{
		size_t addr;
		size_t rem;

		/* init the arg_hdr struct */
		result->hdr.flag      = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts  = longopts;
		result->hdr.datatype  = datatype ? datatype : "<double>";
		result->hdr.glossary  = glossary;
		result->hdr.mincount  = mincount;
		result->hdr.maxcount  = maxcount;
		result->hdr.parent    = result;
		result->hdr.resetfn   = (arg_resetfn*)resetfn_double;
		result->hdr.scanfn    = (arg_scanfn*)scanfn_double;
		result->hdr.checkfn   = (arg_checkfn*)checkfn_double;
		result->hdr.errorfn   = (arg_errorfn*)errorfn_double;

		/* Store the dval[maxcount] array on the first double boundary that immediately follows the arg_dbl struct. */
		/* We do the memory alignment purely for SPARC and Motorola systems. They require floats and doubles to be  */
		/* aligned on natural boundaries */
		addr = (size_t)(result+1);
		rem  = addr % sizeof(double);
		result->dval  = (double*)(addr + sizeof(double) - rem);
		/* printf("addr=%p, dval=%p, sizeof(double)=%d rem=%d\n", addr, result->dval, (int)sizeof(double), (int)rem); */

		result->count = 0;
	}
	/*printf("arg_dbln() returns %p\n",result);*/
	return result;
}

//--------------------------------------------------------------------
//------------------------ arg_end.c -----------------------------
//--------------------------------------------------------------------


static void resetfn_end(struct arg_end *parent)
    {
    /*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
    parent->count = 0;
    }

static void errorfn_end(void *parent, FILE *fp, int error, const char *argval, const char *progname)
    {
    progname = progname ? progname : "";
    argval = argval ? argval : "";

    fprintf(fp,"%s: ",progname);
    switch(error)
        {
        case ARG_ELIMIT:
            fputs("too many errors to display",fp);
            break;
        case ARG_EMALLOC:
            fputs("insufficent memory",fp);
            break;
        case ARG_ENOMATCH:
            fprintf(fp,"unexpected argument \"%s\"",argval);
            break;
        case ARG_EMISSARG:
            fprintf(fp,"option \"%s\" requires an argument",argval);
            break;
        case ARG_ELONGOPT:
            fprintf(fp,"invalid option \"%s\"",argval);
            break;
        default:
            fprintf(fp,"invalid option \"-%c\"",error);
            break;
        }
    fputc('\n',fp);
    }


struct arg_end* arg_end(int maxcount)
    {
    size_t nbytes;
    struct arg_end *result;

    nbytes = sizeof(struct arg_end)
           + maxcount * sizeof(int)             /* storage for int error[maxcount] array*/
           + maxcount * sizeof(void*)           /* storage for void* parent[maxcount] array */
           + maxcount * sizeof(char*);          /* storage for char* argval[maxcount] array */

    result = (struct arg_end*)malloc(nbytes);
    if (result)
        {
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_TERMINATOR;
        result->hdr.shortopts = NULL;
        result->hdr.longopts  = NULL;
        result->hdr.datatype  = NULL;
        result->hdr.glossary  = NULL;
        result->hdr.mincount  = 1;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn*)resetfn_end;
        result->hdr.scanfn    = NULL;
        result->hdr.checkfn   = NULL;
        result->hdr.errorfn   = errorfn_end;

        /* store error[maxcount] array immediately after struct arg_end */
        result->error = (int*)(result+1);

        /* store parent[maxcount] array immediately after error[] array */
        result->parent = (void**)(result->error + maxcount );

        /* store argval[maxcount] array immediately after parent[] array */
        result->argval = (const char**)(result->parent + maxcount );
        }

    /*printf("arg_end(%d) returns %p\n",maxcount,result);*/
    return result;
    }


void arg_print_errors(FILE* fp, struct arg_end* end, const char* progname)
{
	int i;
	/*printf("arg_errors()\n");*/
	for (i=0; i<end->count; i++)
	{
		struct arg_hdr *errorparent = (struct arg_hdr *)(end->parent[i]);
		if (errorparent->errorfn)
			errorparent->errorfn(end->parent[i],fp,end->error[i],end->argval[i],progname);
	}
}

//--------------------------------------------------------------------
//------------------------ arg_file.c -----------------------------
//--------------------------------------------------------------------

#ifdef WIN32
# define FILESEPARATOR1 '\\'
# define FILESEPARATOR2 '/'
#else
# define FILESEPARATOR1 '/'
# define FILESEPARATOR2 '/'
#endif



static void resetfn_file(struct arg_file *parent)
{
/*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
parent->count=0;
}


/* Returns ptr to the base filename within *filename */
static const char* arg_basename_file(const char *filename)
{
const char *result=NULL, *result1, *result2;

/* Find the last occurrence of eother file separator character. */
/* Two alternative file separator chars are supported as legal  */
/* file separators but not both together in the same filename.  */
result1 = (filename ? strrchr(filename,FILESEPARATOR1) : NULL);
result2 = (filename ? strrchr(filename,FILESEPARATOR2) : NULL);

if (result2)
	result=result2+1;   /* using FILESEPARATOR2 (the alternative file separator) */

if (result1)
	result=result1+1;   /* using FILESEPARATOR1 (the preferred file separator) */

if (!result)
	result = filename;  /* neither file separator was found so basename is the whole filename */

/* special cases of "." and ".." are not considered basenames */
if (result && ( strcmp(".",result)==0 || strcmp("..",result)==0 ))
   result = filename + strlen(filename);

return result;
}


/* Returns ptr to the file extension within *basename */
static const char* arg_extension_file(const char *basename)
{
/* find the last occurrence of '.' in basename */
const char *result = (basename ? strrchr(basename,'.') : NULL);

/* if no '.' was found then return pointer to end of basename */
if (basename && !result)
	result = basename+strlen(basename);

/* special case: basenames with a single leading dot (eg ".foo") are not considered as true extensions */
if (basename && result==basename)
	result = basename+strlen(basename);

/* special case: empty extensions (eg "foo.","foo..") are not considered as true extensions */
if (basename && result && result[1]=='\0')
	result = basename+strlen(basename);

return result;
}


static int scanfn_file(struct arg_file *parent, const char *argval)
{
int errorcode = 0;

if (parent->count == parent->hdr.maxcount)
	{
	/* maximum number of arguments exceeded */
	errorcode = EMAXCOUNT_FILE;
	}
else if (!argval)
	{
	/* a valid argument with no argument value was given. */
	/* This happens when an optional argument value was invoked. */
	/* leave parent arguiment value unaltered but still count the argument. */
	parent->count++;
	}
else
	{
	parent->filename[parent->count]  = argval;
	parent->basename[parent->count]  = arg_basename_file(argval);
	parent->extension[parent->count] = arg_extension_file(parent->basename[parent->count]); /* only seek extensions within the basename (not the file path)*/
	parent->count++;
	}

/*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
return errorcode;
}


static int checkfn_file(struct arg_file *parent)
{
	int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_FILE : 0;
	/*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
	return errorcode;
}


static void errorfn_file(struct arg_file *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
{
	const char *shortopts = parent->hdr.shortopts;
	const char *longopts  = parent->hdr.longopts;
	const char *datatype  = parent->hdr.datatype;

	/* make argval NULL safe */
	argval = argval ? argval : "";

	fprintf(fp,"%s: ",progname);
	switch(errorcode)
	{
	case EMINCOUNT_FILE:
		fputs("missing option ",fp);
		arg_print_option(fp,shortopts,longopts,datatype,"\n");
		break;

	case EMAXCOUNT_FILE:
		fputs("excess option ",fp);
		arg_print_option(fp,shortopts,longopts,argval,"\n");
		break;

	default:
		fprintf(fp,"unknown error at \"%s\"\n",argval);
	}
}


struct arg_file* arg_file0(const char* shortopts,
					   const char* longopts,
					   const char *datatype,
					   const char *glossary)
{
	return arg_filen(shortopts,longopts,datatype,0,1,glossary);
}


struct arg_file* arg_file1(const char* shortopts,
					   const char* longopts,
					   const char *datatype,
					   const char *glossary)
{
	return arg_filen(shortopts,longopts,datatype,1,1,glossary);
}


struct arg_file* arg_filen(const char* shortopts,
					   const char* longopts,
					   const char *datatype,
					   int mincount,
					   int maxcount,
					   const char *glossary)
{
	size_t nbytes;
	struct arg_file *result;

/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

	nbytes = sizeof(struct arg_file)     /* storage for struct arg_file */
	   + sizeof(char*) * maxcount    /* storage for filename[maxcount] array */
	   + sizeof(char*) * maxcount    /* storage for basename[maxcount] array */
	   + sizeof(char*) * maxcount;   /* storage for extension[maxcount] array */

	result = (struct arg_file*)malloc(nbytes);
	if (result)
	{
		int i;

		/* init the arg_hdr struct */
		result->hdr.flag      = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts  = longopts;
		result->hdr.glossary  = glossary;
		result->hdr.datatype  = datatype ? datatype : "<file>";
		result->hdr.mincount  = mincount;
		result->hdr.maxcount  = maxcount;
		result->hdr.parent    = result;
		result->hdr.resetfn   = (arg_resetfn*)resetfn_file;
		result->hdr.scanfn    = (arg_scanfn*)scanfn_file;
		result->hdr.checkfn   = (arg_checkfn*)checkfn_file;
		result->hdr.errorfn   = (arg_errorfn*)errorfn_file;

		/* store the filename,basename,extension arrays immediately after the arg_file struct */
		result->filename  = (const char**)(result+1);
		result->basename  = result->filename + maxcount;
		result->extension = result->basename + maxcount;
		result->count = 0;

		/* foolproof the string pointers by initialising them with empty strings */
		for (i=0; i<maxcount; i++)
		{
			result->filename[i] = "";
			result->basename[i] = "";
			result->extension[i] = "";
		}
	}
	/*printf("arg_filen() returns %p\n",result);*/
	return result;
}



//--------------------------------------------------------------------
//------------------------ arg_int.c -----------------------------
//--------------------------------------------------------------------

#include <limits.h>

static void resetfn_int(struct arg_int *parent)
    {
    /*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
    parent->count=0;
    }

/* strtol0x() is like strtol() except that the numeric string is    */
/* expected to be prefixed by "0X" where X is a user supplied char. */
/* The string may optionally be prefixed by white space and + or -  */
/* as in +0X123 or -0X123.                                          */
/* Once the prefix has been scanned, the remainder of the numeric   */
/* string is converted using strtol() with the given base.          */
/* eg: to parse hex str="-0X12324", specify X='X' and base=16.      */
/* eg: to parse oct str="+0o12324", specify X='O' and base=8.       */
/* eg: to parse bin str="-0B01010", specify X='B' and base=2.       */
/* Failure of conversion is indicated by result where *endptr==str. */
static long int strtol0X_int(const char* str, const char **endptr, char X, int base)
    {
    long int val;               /* stores result */
    int s=1;                    /* sign is +1 or -1 */
    const char *ptr=str;        /* ptr to current position in str */

    /* skip leading whitespace */
    while (isspace(*ptr))
        ptr++;
    /* printf("1) %s\n",ptr); */

    /* scan optional sign character */
    switch (*ptr)
        {
        case '+':
            ptr++;
            s=1;
            break;
        case '-':
            ptr++;
            s=-1;
            break;
        default:
            s=1;
            break;
        }
    /* printf("2) %s\n",ptr); */

    /* '0X' prefix */
    if ((*ptr++)!='0')
        {
        /* printf("failed to detect '0'\n"); */
        *endptr=str;
        return 0;
        }
   /* printf("3) %s\n",ptr); */
   if (toupper(*ptr++)!=toupper(X))
        {
        /* printf("failed to detect '%c'\n",X); */
        *endptr=str;
        return 0;
        }
    /* printf("4) %s\n",ptr); */

    /* attempt conversion on remainder of string using strtol() */
    val = strtol(ptr,(char**)endptr,base);
    if (*endptr==ptr)
        {
        /* conversion failed */
        *endptr=str;
        return 0;
        }

    /* success */
    return s*val;
    }


/* Returns 1 if str matches suffix (case insensitive).    */
/* Str may contain trailing whitespace, but nothing else. */
static int detectsuffix_int(const char *str, const char *suffix)
    {
    /* scan pairwise through strings until mismatch detected */
    while( toupper(*str) == toupper(*suffix) )
        {
        /* printf("'%c' '%c'\n", *str, *suffix); */

        /* return 1 (success) if match persists until the string terminator */
        if (*str=='\0')
           return 1;

        /* next chars */
        str++;
        suffix++;
        }
    /* printf("'%c' '%c' mismatch\n", *str, *suffix); */

    /* return 0 (fail) if the matching did not consume the entire suffix */
    if (*suffix!=0)
        return 0;   /* failed to consume entire suffix */

    /* skip any remaining whitespace in str */
    while (isspace(*str))
        str++;

    /* return 1 (success) if we have reached end of str else return 0 (fail) */
    return (*str=='\0') ? 1 : 0;
    }


static int scanfn_int(struct arg_int *parent, const char *argval)
    {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
        {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT_INT;
        }
    else if (!argval)
        {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
        }
    else
        {
        long int val;
        const char *end;

        /* attempt to extract hex integer (eg: +0x123) from argval into val conversion */
        val = strtol0X_int(argval, &end, 'X', 16);
        if (end==argval)
            {
            /* hex failed, attempt octal conversion (eg +0o123) */
            val = strtol0X_int(argval, &end, 'O', 8);
            if (end==argval)
                {
                /* octal failed, attempt binary conversion (eg +0B101) */
                val = strtol0X_int(argval, &end, 'B', 2);
                if (end==argval)
                    {
                    /* binary failed, attempt decimal conversion with no prefix (eg 1234) */
                    val = strtol(argval, (char**)&end, 10);
                    if (end==argval)
                        {
                        /* all supported number formats failed */
                        return EBADINT;
                        }
                    }
                }
            }

        /* Safety check for integer overflow. WARNING: this check    */
        /* achieves nothing on machines where size(int)==size(long). */
        if ( val>INT_MAX || val<INT_MIN )
            errorcode = EOVERFLOW_INT;

        /* Detect any suffixes (KB,MB,GB) and multiply argument value appropriately. */
        /* We need to be mindful of integer overflows when using such big numbers.   */
        if (detectsuffix_int(end,"KB"))             /* kilobytes */
            {
            if ( val>(INT_MAX/1024) || val<(INT_MIN/1024) )
                errorcode = EOVERFLOW_INT;          /* Overflow would occur if we proceed */
            else
                val*=1024;                      /* 1KB = 1024 */
            }
        else if (detectsuffix_int(end,"MB"))        /* megabytes */
            {
            if ( val>(INT_MAX/1048576) || val<(INT_MIN/1048576) )
                errorcode = EOVERFLOW_INT;          /* Overflow would occur if we proceed */
            else
                val*=1048576;                   /* 1MB = 1024*1024 */
            }
        else if (detectsuffix_int(end,"GB"))        /* gigabytes */
            {
            if ( val>(INT_MAX/1073741824) || val<(INT_MIN/1073741824) )
                errorcode = EOVERFLOW_INT;          /* Overflow would occur if we proceed */
            else
                val*=1073741824;                /* 1GB = 1024*1024*1024 */
            }
        else if (!detectsuffix_int(end,""))
            errorcode = EBADINT;                /* invalid suffix detected */

        /* if success then store result in parent->ival[] array */
        if (errorcode==0)
            parent->ival[parent->count++] = val;
        }

    /* printf("%s:scanfn(%p,%p) returns %d\n",__FILE__,parent,argval,errorcode); */
    return errorcode;
    }

static int checkfn_int(struct arg_int *parent)
    {
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_INT : 0;
    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static void errorfn_int(struct arg_int *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
    {
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp,"%s: ",progname);
    switch(errorcode)
        {
        case EMINCOUNT_INT:
            fputs("missing option ",fp);
            arg_print_option(fp,shortopts,longopts,datatype,"\n");
            break;

        case EMAXCOUNT_INT:
            fputs("excess option ",fp);
            arg_print_option(fp,shortopts,longopts,argval,"\n");
            break;

        case EBADINT:
            fprintf(fp,"invalid argument \"%s\" to option ",argval);
            arg_print_option(fp,shortopts,longopts,datatype,"\n");
            break;

        case EOVERFLOW_INT:
            fputs("integer overflow at option ",fp);
            arg_print_option(fp,shortopts,longopts,datatype," ");
            fprintf(fp,"(%s is too large)\n",argval);
            break;
        }
    }


struct arg_int* arg_int0(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         const char *glossary)
    {
    return arg_intn(shortopts,longopts,datatype,0,1,glossary);
    }

struct arg_int* arg_int1(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         const char *glossary)
    {
    return arg_intn(shortopts,longopts,datatype,1,1,glossary);
    }


struct arg_int* arg_intn(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         int mincount,
                         int maxcount,
                         const char *glossary)
    {
    size_t nbytes;
    struct arg_int *result;

	/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_int)     /* storage for struct arg_int */
           + maxcount * sizeof(int);    /* storage for ival[maxcount] array */

    result = (struct arg_int*)malloc(nbytes);
    if (result)
        {
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : "<int>";
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn*)resetfn_int;
        result->hdr.scanfn    = (arg_scanfn*)scanfn_int;
        result->hdr.checkfn   = (arg_checkfn*)checkfn_int;
        result->hdr.errorfn   = (arg_errorfn*)errorfn_int;

        /* store the ival[maxcount] array immediately after the arg_int struct */
        result->ival  = (int*)(result+1);
        result->count = 0;
        }
    /*printf("arg_intn() returns %p\n",result);*/
    return result;
    }

//--------------------------------------------------------------------
//------------------------ arg_lit.c -----------------------------
//--------------------------------------------------------------------



static void resetfn_lit(struct arg_lit *parent)
{
/*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
parent->count = 0;
}

static int scanfn_lit(struct arg_lit *parent, const char *argval)
{
int errorcode = 0;
if (parent->count < parent->hdr.maxcount )
	parent->count++;
else
	errorcode = EMAXCOUNT_LIT;
/*printf("%s:scanfn(%p,%s) returns %d\n",__FILE__,parent,argval,errorcode);*/
return errorcode;
}

static int checkfn_lit(struct arg_lit *parent)
{
int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_LIT : 0;
/*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
return errorcode;
}

static void errorfn_lit(struct arg_lit *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
{
const char *shortopts = parent->hdr.shortopts;
const char *longopts  = parent->hdr.longopts;
const char *datatype  = parent->hdr.datatype;

switch(errorcode)
	{
	case EMINCOUNT_LIT:
		fprintf(fp,"%s: missing option ",progname);
		arg_print_option(fp,shortopts,longopts,datatype,"\n");
		fprintf(fp,"\n");
		break;

	case EMAXCOUNT_LIT:
		fprintf(fp,"%s: extraneous option ",progname);
		arg_print_option(fp,shortopts,longopts,datatype,"\n");
		break;
	}
}

struct arg_lit* arg_lit0(const char* shortopts,
					 const char* longopts,
					 const char* glossary)
{return arg_litn(shortopts,longopts,0,1,glossary);}

struct arg_lit* arg_lit1(const char* shortopts,
					 const char* longopts,
					 const char* glossary)
{return arg_litn(shortopts,longopts,1,1,glossary);}


struct arg_lit* arg_litn(const char* shortopts,
					 const char* longopts,
					 int mincount,
					 int maxcount,
					 const char *glossary)
{
struct arg_lit *result;

/* foolproof things by ensuring maxcount is not less than mincount */
maxcount = (maxcount<mincount) ? mincount : maxcount;

result = (struct arg_lit*)malloc(sizeof(struct arg_lit));
if (result)
	{
	/* init the arg_hdr struct */
	result->hdr.flag      = 0;
	result->hdr.shortopts = shortopts;
	result->hdr.longopts  = longopts;
	result->hdr.datatype  = NULL;
	result->hdr.glossary  = glossary;
	result->hdr.mincount  = mincount;
	result->hdr.maxcount  = maxcount;
	result->hdr.parent    = result;
	result->hdr.resetfn   = (arg_resetfn*)resetfn_lit;
	result->hdr.scanfn    = (arg_scanfn*)scanfn_lit;
	result->hdr.checkfn   = (arg_checkfn*)checkfn_lit;
	result->hdr.errorfn   = (arg_errorfn*)errorfn_lit;

	/* init local variables */
	result->count = 0;
	}
/*printf("arg_litn() returns %p\n",result);*/
return result;
}

//--------------------------------------------------------------------
//------------------------ arg_rem.c -----------------------------
//--------------------------------------------------------------------


struct arg_rem* arg_rem(const char *datatype,
                        const char *glossary)
    {
    struct arg_rem *result = (struct arg_rem*)malloc(sizeof(struct arg_rem));
    if (result)
        {
        /* init the arg_hdr struct */
        result->hdr.flag      = 0;
        result->hdr.shortopts = NULL;
        result->hdr.longopts  = NULL;
        result->hdr.datatype  = datatype;
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = 1;
        result->hdr.maxcount  = 1;
        result->hdr.parent    = result;
        result->hdr.resetfn   = NULL;
        result->hdr.scanfn    = NULL;
        result->hdr.checkfn   = NULL;
        result->hdr.errorfn   = NULL;
        }
    /*printf("arg_rem() returns %p\n",result);*/
    return result;
    }


//--------------------------------------------------------------------
//------------------------ arg_rex.c -----------------------------
//--------------------------------------------------------------------

#include <regex.h>

/* local error codes (these must not conflict with reg_error codes) */

struct privhdr
    {
    const char *pattern;
    int flags;
    regex_t regex;
    };


static void resetfn_rex(struct arg_rex *parent)
    {
    struct privhdr *priv = (struct privhdr*)(parent->hdr.priv);

    /*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
    parent->count=0;

    /* construct the regex representation of the given pattern string. */
    /* Dont bother checking for errors as we already did that earlier (in the constructor) */
    regcomp(&(priv->regex), priv->pattern, priv->flags);
    }

static int scanfn_rex(struct arg_rex *parent, const char *argval)
    {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount )
        {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT_REX;
        }
    else if (!argval)
        {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
        }
    else
        {
        struct privhdr *priv = (struct privhdr*)parent->hdr.priv;

       /* test the current argument value for a match with the regular expression */
        /* if a match is detected, record the argument value in the arg_rex struct */
        errorcode = regexec(&(priv->regex), argval, 0, NULL, 0);
        if (errorcode==0)
            parent->sval[parent->count++] = argval;
        }

    /*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static int checkfn_rex(struct arg_rex *parent)
    {
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_REX : 0;
    struct privhdr *priv = (struct privhdr*)parent->hdr.priv;

    /* free the regex "program" we constructed in resetfn */
    regfree(&(priv->regex));

    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static void errorfn_rex(struct arg_rex *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
    {
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp,"%s: ",progname);
    switch(errorcode)
        {
        case EMINCOUNT_REX:
            fputs("missing option ",fp);
            arg_print_option(fp,shortopts,longopts,datatype,"\n");
            break;

        case EMAXCOUNT_REX:
            fputs("excess option ",fp);
            arg_print_option(fp,shortopts,longopts,argval,"\n");
            break;

        case REG_NOMATCH:
            fputs("illegal value  ",fp);
            arg_print_option(fp,shortopts,longopts,argval,"\n");
            break;

        default:
            {
            char errbuff[256];
            regerror(errorcode, NULL, errbuff, sizeof(errbuff));
            printf("%s\n", errbuff);
            }
            break;
        }
    }


struct arg_rex* arg_rex0(const char* shortopts,
                         const char* longopts,
                         const char* pattern,
                         const char *datatype,
                         int flags,
                         const char *glossary)
    {
    return arg_rexn(shortopts,longopts,pattern,datatype,0,1,flags,glossary);
    }

struct arg_rex* arg_rex1(const char* shortopts,
                         const char* longopts,
                         const char* pattern,
                         const char *datatype,
                         int flags,
                         const char *glossary)
    {
    return arg_rexn(shortopts,longopts,pattern,datatype,1,1,flags,glossary);
    }


struct arg_rex* arg_rexn(const char* shortopts,
                         const char* longopts,
                         const char* pattern,
                         const char *datatype,
                         int mincount,
                         int maxcount,
                         int flags,
                         const char *glossary)
    {
    size_t nbytes;
    struct arg_rex *result;
    struct privhdr *priv;

    if (!pattern)
        {
        printf("argtable: ERROR - illegal regular expression pattern \"(NULL)\"\n");
        printf("argtable: Bad argument table.\n");
        return NULL;
        }

	/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_rex)       /* storage for struct arg_rex */
           + sizeof(struct privhdr)       /* storage for private arg_rex data */
           + maxcount * sizeof(char*);    /* storage for sval[maxcount] array */

    result = (struct arg_rex*)malloc(nbytes);
    if (result)
        {
        int errorcode, i;

        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : pattern;
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn*)resetfn_rex;
        result->hdr.scanfn    = (arg_scanfn*)scanfn_rex;
        result->hdr.checkfn   = (arg_checkfn*)checkfn_rex;
        result->hdr.errorfn   = (arg_errorfn*)errorfn_rex;

        /* store the arg_rex_priv struct immediately after the arg_rex struct */
        result->hdr.priv  = (const char**)(result+1);
        priv = (struct privhdr*)(result->hdr.priv);
        priv->pattern = pattern;
        priv->flags = flags | REG_NOSUB;

        /* store the sval[maxcount] array immediately after the arg_rex_priv struct */
        result->sval  = (const char**)(priv+1);
        result->count = 0;

        /* foolproof the string pointers by initialising them to reference empty strings */
        for (i=0; i<maxcount; i++)
            { result->sval[i] = ""; }

        /* here we construct and destroy a regex representation of the regular expression
           for no other reason than to force any regex errors to be trapped now rather
           than later. If we dont, then errors may go undetected until an argument is
           actually parsed. */
        errorcode = regcomp(&(priv->regex), priv->pattern, priv->flags);
        if (errorcode)
            {
            char errbuff[256];
            regerror(errorcode, &(priv->regex), errbuff, sizeof(errbuff));
            printf("argtable: %s \"%s\"\n", errbuff, priv->pattern);
            printf("argtable: Bad argument table.\n");
            }
        else
            regfree(&(priv->regex));
        }

    /*printf("arg_rexn() returns %p\n",result);*/
    return result;
    }



//--------------------------------------------------------------------
//------------------------ arg_str.c -----------------------------
//--------------------------------------------------------------------

/* local error codes */
enum {EMINCOUNT_STR=1,EMAXCOUNT_STR};

static void resetfn_str(struct arg_str *parent)
    {
    /*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
    parent->count=0;
    }

static int scanfn_str(struct arg_str *parent, const char *argval)
    {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
        {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT_STR;
        }
    else if (!argval)
        {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
        }
    else
        {
        parent->sval[parent->count++] = argval;
        }

    /*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static int checkfn_str(struct arg_str *parent)
    {
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT_STR : 0;
    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static void errorfn_str(struct arg_str *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
    {
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp,"%s: ",progname);
    switch(errorcode)
        {
        case EMINCOUNT_STR:
            fputs("missing option ",fp);
            arg_print_option(fp,shortopts,longopts,datatype,"\n");
            break;

        case EMAXCOUNT_STR:
            fputs("excess option ",fp);
            arg_print_option(fp,shortopts,longopts,argval,"\n");
            break;
        }
    }


struct arg_str* arg_str0(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         const char *glossary)
    {
    return arg_strn(shortopts,longopts,datatype,0,1,glossary);
    }

struct arg_str* arg_str1(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         const char *glossary)
    {
    return arg_strn(shortopts,longopts,datatype,1,1,glossary);
    }


struct arg_str* arg_strn(const char* shortopts,
                         const char* longopts,
                         const char *datatype,
                         int mincount,
                         int maxcount,
                         const char *glossary)
    {
    size_t nbytes;
    struct arg_str *result;

	/* foolproof things by ensuring maxcount is not less than mincount */
	maxcount = (maxcount<mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_str)     /* storage for struct arg_str */
           + maxcount * sizeof(char*);  /* storage for sval[maxcount] array */

    result = (struct arg_str*)malloc(nbytes);
    if (result)
        {
        int i;

        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : "<string>";
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn*)resetfn_str;
        result->hdr.scanfn    = (arg_scanfn*)scanfn_str;
        result->hdr.checkfn   = (arg_checkfn*)checkfn_str;
        result->hdr.errorfn   = (arg_errorfn*)errorfn_str;

        /* store the sval[maxcount] array immediately after the arg_str struct */
        result->sval  = (const char**)(result+1);
        result->count = 0;

        /* foolproof the string pointers by initialising them to reference empty strings */
        for (i=0; i<maxcount; i++)
            { result->sval[i] = ""; }
        }
    /*printf("arg_strn() returns %p\n",result);*/
    return result;
    }

//--------------------------------------------------------------------
//------------------------ arg_str.c -----------------------------
//--------------------------------------------------------------------

