/**
 * @file
 * @author      Kelemen Tamas <kiskele@krc.hu>
 * @brief This file is used to check CLI commands and their parameters.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <arpa/inet.h>
#include "lib_descs.h"
#include "sys_ctrls.h"
#include "lib_funcs.h"
#include "etc_cli.h"
#include "dmap_sel.h"

/**
 * Constructor that allocates memory and initializes the structure of "interface up|down" command
 *
 * @note This memory needs to deallocate after use. Use mpt_int_destructor()
 *
 * @param this   Pointer to an "mpt_int *" variable
 */
void mpt_int_constructor(struct mpt_int ** this) {
    if((*this = (struct mpt_int *)malloc(sizeof(struct mpt_int))) == NULL) return;
    (*this)->interface = NULL;
    (*this)->mark = NULL;
}

/**
 * Destructor that deallocates memory after struct mpt_int * is no longer needed
 *
 * @param this   Pointer to an "mpt_int *" variable
 */
void mpt_int_destructor(struct mpt_int ** this) {
    if(*this == NULL) return;

    if((*this)->interface) free((*this)->interface);
    if((*this)->mark) free((*this)->mark);
    free(*this);
}

/**
 * Constructor that allocates memory and initializes the structure of "address add|del" command
 *
 * @note This memory needs to deallocate after use. Use mpt_addr_destructor()
 *
 * @param this   Pointer to an "mpt_addr *" variable
 */
void mpt_addr_constructor(struct mpt_addr ** this) {
    if((*this = (struct mpt_addr *)malloc(sizeof(struct mpt_addr))) == NULL) return;
    (*this)->ip = NULL;
    (*this)->op = NULL;
    (*this)->mask = NULL;
    (*this)->dev = NULL;
}

/**
 * Destructor that deallocates memory after struct mpt_addr * is no longer needed
 *
 * @param this   Pointer to an "mpt_addr *" variable
 */
void mpt_addr_destructor(struct mpt_addr ** this) {
    if(*this == NULL) return;

    if((*this)->ip) free((*this)->ip);
    if((*this)->op) free((*this)->op);
    if((*this)->mask) free((*this)->mask);
    if((*this)->dev) free((*this)->dev);
    free(*this);
}

/**
 * Constructor that allocates memory and initializes the structure of "path up|down" command
 *
 * @note This memory needs to deallocate after use. Use mpt_change_path_destructor()
 *
 * @param this   Pointer to an "mpt_change_path *" variable
 */
void mpt_change_path_constructor(struct mpt_change_path ** this) {
    if((*this = (struct mpt_change_path *)malloc(sizeof(struct mpt_change_path))) == NULL) return;
    (*this)->op = NULL;
    memset((*this)->local_ip, 0, sizeof((*this)->local_ip));
    memset((*this)->remote_ip, 0, sizeof((*this)->remote_ip));
}

/**
 * Destructor that deallocates memory after struct mpt_change_path * is no longer needed
 *
 * @param this   Pointer to an "mpt_change_path *" variable
 */
void mpt_change_path_destructor(struct mpt_change_path ** this) {
    if(*this == NULL) return;

    if((*this)->op) free((*this)->op);
    free(*this);
}

/**
 * Function that creates string from regular expression match
 *
 * @note This function allocates memory for dst string that needs to deallocate after use, but it is done in the destructor functions.
 *
 * @param dst    Pointer to the newly allocated destination string
 * @param src    Original string which contains the matching string
 * @param pmatch Which match need to be saved. N-th element of array return by regexec \n
 *               This contains index of the beginning and the end of the match
 */
void save_match(char ** dst, char * src, regmatch_t pmatch) {
    if(pmatch.rm_so >= 0) {
        *dst = (char*)malloc(pmatch.rm_eo - pmatch.rm_so + 1);
        strncpy(*dst, src+pmatch.rm_so, pmatch.rm_eo - pmatch.rm_so);
        *(*dst + pmatch.rm_eo - pmatch.rm_so) = 0;
    } else {
        *dst = NULL;
    }
}

/**
 * Function to parse CLI input and return command type and structure of parameters.
 *
 * @param cmd      Input string which need to parse
 * @param rettype  Return the matched command. See @ref CMDTYPE.
 * @param ret      Return a struct for that command that contains the parsed arguments
 *
 * @return         PARSE_OK if the command and parameters seem ok, otherwise the code of parameter error. See @ref ReturnParser for return values
 */
int parser(char * cmd, CMDTYPE * rettype, void ** ret) {
    int errcode, rv = PARSE_ERR_NOCMD, i, j;
    regex_t preg;
    char * regexps[] = {
/// <b> CLI commands: </b>
#define CLI_COMMAND(name, regex) regex,
#include "etc_cli.def"
#undef CLI_COMMAND
        NULL    // end of array
    };

    for(i=0; regexps[i]!=NULL; i++) {
        int domain;
        unsigned char buf[sizeof(struct in6_addr)];
        int pmatchcnt = 1;
        regmatch_t * pmatch = NULL;
        if((errcode=regcomp(&preg, regexps[i], REG_EXTENDED))) {
            char errstr[1024];
            regerror(errcode, &preg, errstr, sizeof(errstr));
            fprintf(stderr, "%s: Invalid regular expression \"%s\": %s\n", __FUNCTION__, regexps[i], errstr);
            return PARSE_ERR_REGEX;
        }

        for(j=0; j<strlen(regexps[i]); j++) {
            if(regexps[i][j] == '(') pmatchcnt++;
        }
        if(pmatchcnt > 0) pmatch = (regmatch_t *) malloc(pmatchcnt*sizeof(regmatch_t));

        if(!regexec(&preg, cmd, pmatchcnt, pmatch, 0)) {
            rv = PARSE_OK;
            *rettype = i;
            *ret = NULL;
            switch(i) {
                case NTRT_INT:
                    if(pmatch[3].rm_so >= 0) {
                        struct mpt_int * ret_int = NULL;

                        mpt_int_constructor(&ret_int);
                        if(ret_int == NULL) break;
                        *ret = ret_int;

                        save_match(&(ret_int->interface), cmd, pmatch[3]);
                        save_match(&(ret_int->mark), cmd, pmatch[4]);
                    } else rv = PARSE_ERR_PARAMS;
                    break;
                case NTRT_ADDR:
                    if(pmatch[3].rm_so >= 0) {
                        struct mpt_addr * ret_addr = NULL;

                        mpt_addr_constructor(&ret_addr);
                        if(ret_addr == NULL) break;
                        *ret = ret_addr;

                        save_match(&(ret_addr->op), cmd, pmatch[3]);
                        save_match(&(ret_addr->ip), cmd, pmatch[4]);
                        save_match(&(ret_addr->mask), cmd, pmatch[10]);
                        save_match(&(ret_addr->dev), cmd, pmatch[11]);

                        // test ip address
                        if(pmatch[5].rm_so >= 0) domain = AF_INET;
                        else domain = AF_INET6;
                        if(inet_pton(domain, ret_addr->ip, buf) <= 0) {
                            rv = PARSE_ERR_IP;
                            break;
                        }
                        inet_ntop(domain, buf, ret_addr->ip, strlen(ret_addr->ip));

                        // test mask value
                        if(ret_addr->mask) {
                            if(domain == AF_INET) {
                                if(atoi(ret_addr->mask)>32) {
                                    rv = PARSE_ERR_MASK;
                                }
                            } else {
                                if(atoi(ret_addr->mask)>128) {
                                    rv = PARSE_ERR_MASK;
                                }
                            }
                        }
                    } else rv = PARSE_ERR_PARAMS;
                    break;
                case NTRT_RELOAD:
                case NTRT_SAVE:
                    // commands with 1 optional parameter
                    if(pmatch[2].rm_so >= 0) {
                        save_match((char**)ret, cmd, pmatch[2]);
                    } else if(pmatch[1].rm_eo - pmatch[1].rm_so > 0) rv = PARSE_ERR_PARAMS;
                    break;
                case NTRT_DELETE:
                    // commands with 1 mandatory parameter
                    if(pmatch[2].rm_so >= 0) {
                        save_match((char**)ret, cmd, pmatch[2]);
                    } else rv = PARSE_ERR_PARAMS;
                    break;
                case NTRT_PATH:
                    {
                        char * temp;
                        int start;
                        struct mpt_change_path * ret_path = NULL;
                        mpt_change_path_constructor(&ret_path);

                        if(ret_path == NULL) break;

                        *ret = ret_path;

                        if(pmatch[2].rm_so < 0) {
                            rv = PARSE_ERR_PARAMS;
                            break;
                        }

                        save_match(&(ret_path->op), cmd, pmatch[2]);

                        save_match(&temp, cmd, pmatch[3]);
                        if(pmatch[4].rm_so >= 0) {
                            domain = AF_INET;
                            ret_path->local_ip[2] = 0xFFFF0000;
                            start = 3;
                        } else {
                            domain = AF_INET6;
                            start = 0;
                        }
                        if(inet_pton(domain, temp, &(ret_path->local_ip[start])) <= 0) {
                            rv = PARSE_ERR_IP;
                            free(temp);
                            break;
                        }
                        free(temp);

                        save_match(&temp, cmd, pmatch[8]);
                        if(pmatch[9].rm_so >= 0) {
                            domain = AF_INET;
                            ret_path->remote_ip[2] = 0xFFFF0000;
                            start = 3;
                        } else {
                            domain = AF_INET6;
                            start = 0;
                        }
                        if(inet_pton(domain, temp, &(ret_path->remote_ip[start])) <= 0) {
                            rv = PARSE_ERR_IP;
                            free(temp);
                            break;
                        }
                        free(temp);
                    }
                    break;
                default:
                    break;
            }

            if(pmatch) free(pmatch);
            regfree(&preg);
            return rv;
        }
        if(pmatch) free(pmatch);
        regfree(&preg);
    }

    // invalid command
    *rettype = NTRT_NOCMD;
    return rv;
}

/**
 * Check CLI command syntax on client side to send out only valid commands
 *
 * @param cmd   CLI command input
 * @return      See @ref ReturnParser
 */
int parse_cmd(char * cmd) {
    int rv;
    CMDTYPE rettype = NTRT_NOCMD;
    void * ret = NULL;

    rv=parser(cmd, &rettype, &ret);

    switch(rv) {
        case PARSE_OK:
        case PARSE_ERR_REGEX:
            break;
        case PARSE_ERR_IP:
            INFOPRINT("ERROR: Invalid IP specified\n");
            break;
        case PARSE_ERR_MASK:
        	INFOPRINT("ERROR: Invalid netmask specified\n");
            break;
        case PARSE_ERR_PARAMS:
        	INFOPRINT("ERROR: Wrong parameters specified\n");
            if(rettype == NTRT_INT) printf(HELP_INT);
            else if(rettype == NTRT_ADDR) printf(HELP_ADDR);
            else if(rettype == NTRT_RELOAD) printf(HELP_RELOAD);
            else if(rettype == NTRT_DELETE) printf(HELP_DELETE);
            else if(rettype == NTRT_SAVE) printf(HELP_SAVE);
            else if(rettype == NTRT_PATH) printf(HELP_PATH);
            else INFOPRINT(HELP);
            break;
        default:
        	INFOPRINT("ERROR: Illegal command\n");
        	INFOPRINT(HELP);
    }

    switch(rettype) {
        case NTRT_INT:
            mpt_int_destructor((struct mpt_int **)&ret);
            break;
        case NTRT_ADDR:
            mpt_addr_destructor((struct mpt_addr **)&ret);
            break;
        case NTRT_PATH:
            mpt_change_path_destructor((struct mpt_change_path **)&ret);
            break;
        case NTRT_RELOAD:
        case NTRT_DELETE:
        case NTRT_SAVE:
            free(ret);
            break;
        default:
            break;
    }

    return rv;
}

#define NTRT_SERVER
#ifdef NTRT_SERVER
/**
 * Check CLI command syntax on server side and execute that
 * @todo Check result of commands and if something went wrong return an error code and put some explanation in cmd parameter.
 *
 * @param cmd CLI command received
 * @return Execution of command failed or succeeded
 */
//int exec_cmd(char * cmd, int sock, struct sockaddr * client, unsigned int csize) {
command_t* make_cmd_from_str(char * cmd, int32_t *rv) {
    CMDTYPE rettype = NTRT_NOCMD;
	int32_t       index;
	char_t        part[255];
    command_t    *result = NULL;
    interface_t  *interface;
    path_t       *path;
    void * ret = NULL;
    char cmdstr[256];

    *rv = PARSE_ERR_NOCMD;
    *rv=parser(cmd, &rettype, &ret);
    switch(rettype) {
        case NTRT_INT:
        	if(*rv != PARSE_OK) {
        		return NULL;
        	}
			struct mpt_int * ret_int = ret;
			if((interface = dmap_get_inf_byname(ret_int->interface, NULL)) == NULL){
				mpt_int_destructor((struct mpt_int **)&ret);
				return NULL;
			}
			if(strcmp(ret_int->mark, "down") == 0) {
				result = make_event_cmd(NTRT_EVENT_INTERFACE_DOWN, interface);
			} else {
				result = make_event_cmd(NTRT_EVENT_INTERFACE_UP, interface);
			}
            mpt_int_destructor((struct mpt_int **)&ret);
            break;
        case NTRT_ADDR:
        	if(*rv != PARSE_OK) {
        		return NULL;
        	}

			struct mpt_addr * ret_addr = ret;
			sprintf(cmdstr, "%s/mpt_addr_adddel.sh %s %s", sysio->bash_dir, ret_addr->op, ret_addr->ip);
			if(ret_addr->mask) sprintf(cmdstr+strlen(cmdstr), "/%s", ret_addr->mask);
			else sprintf(cmdstr+strlen(cmdstr), "/24");
			sprintf(cmdstr+strlen(cmdstr), " %s", ret_addr->dev);
			result = make_cmd_opcall("Address uop/down command.", cmdstr);

            mpt_addr_destructor((struct mpt_addr **)&ret);
            break;
        case NTRT_RELOAD:
			result = make_event_cmd(NTRT_EVENT_STOP, NULL);
			cmdcat(result, make_event_cmd(NTRT_EVENT_CONNECTION_RELOAD, ret));
			cmdcat(result, make_event_cmd(NTRT_EVENT_START, NULL));
            break;
        case NTRT_DELETE:
			result = make_event_cmd(NTRT_EVENT_STOP, NULL);
			cmdcat(result, make_event_cmd(NTRT_EVENT_CONNECTION_DELETE, ret));
			cmdcat(result, make_event_cmd(NTRT_EVENT_START, NULL));
            break;
        case NTRT_SAVE:
        	DEBUGPRINT("found command is mpt event connection save (%s)", ret);
			result = make_event_cmd(NTRT_EVENT_CONNECTION_SAVE, ret);
            break;
        case NTRT_PATH:
        	if(*rv != PARSE_OK) {
        		return NULL;
        	}
        	struct mpt_change_path * ret_path = ret;
        	path = dmap_get_pth_byip(ret_path->local_ip, ret_path->remote_ip, NULL);
        	if(path == NULL){
        		return NULL;
        	}
        	if(strcmp(ret_path->op, "up") == 0){
        		result = make_event_cmd(NTRT_EVENT_PATH_UP, path);
        	}else if(strcmp(ret_path->op, "down")){
        		result = make_event_cmd(NTRT_EVENT_PATH_DOWN, path);
        	}
        	mpt_change_path_destructor((struct mpt_change_path **)&ret);
            break;
        case NTRT_STATUS:
        	/*
            {
                connection_type * p = mp_global_conn;
                char buff[2048];        // This is the receive buffer size too and filled per connections, but was 1528 at testing
                FILE * fd;

                fd = fmemopen(buff, 2048, "wb");

                while(p) {
                    memset(buff, 0, 2048);
                    conn_print_item(fd, p);
                    fflush(fd);
                    if(buff[0] != 0) sendto(sock, buff, strlen(buff), 0, client, csize);
                    p = p->next;
                }

                fclose(fd);
            }
            */
            break;
        default:

        	INFOPRINT("Unrecognized command");
        	sprintf(cmdstr, "");
        	for(index = 0; index < 32; ++index)
        	{
        		sprintf(part, "%X ", cmd[index]);
        		strcat(cmdstr, part);
        	}
        	DEBUGPRINT("Command: %s (%s)", &(cmd[1]), cmdstr);
            break;
    }

    return result;
}
#endif
