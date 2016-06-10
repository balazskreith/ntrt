/*
 * etc_cli.h
 *
 *  Created on: Apr 15, 2014
 *      Author: balazs
 */

#ifndef INCGUARD_ETC_CLI_H_
#define INCGUARD_ETC_CLI_H_


#define strstart(src, pattern) strncmp(src, pattern, strlen(pattern))   ///< Src string begins with pattern

#define REGEX_IPV4 "([[:digit:]]{1,3})\\.([[:digit:]]{1,3})\\.([[:digit:]]{1,3})\\.([[:digit:]]{1,3})"
                                                                        ///< Regexp pattern to match a part of IPv4 address
#define REGEX_IPV6 "[0-9a-fA-F:\\.]{1,4}"                               ///< Regexp pattern to match a part of IPv6 address

int parse_cmd(char * cmd);
//int exec_cmd(char * cmd, int sock, struct sockaddr * client, unsigned int csize);
command_t* make_cmd_from_str(char * cmd, int32_t *rv);
/**
 * @defgroup ReturnParser Parser Errors
 * @brief Return values of parser()
 * @{
 */
#define PARSE_OK            0   ///< Successfully parsed and parameters seems good
#define PARSE_ERR_NOCMD     1   ///< No such command
#define PARSE_ERR_REGEX     2   ///< Invalid regexp format specified in c source
#define PARSE_ERR_IP        3   ///< Wrong IP address entered
#define PARSE_ERR_MASK      4   ///< Wrong network mask entered
#define PARSE_ERR_PARAMS    5   ///< Wrong parameter list

/** @} */


/**
 * @brief The parser() function sets in its 2nd parameter that witch command were recognised
 * These values are the corresponding indexes of the successfully matched regular expressions or NTRT_NOCMD if nothing was matched.
 */
typedef enum {
#define CLI_COMMAND(name, regex) name,
	CLI_COMMAND( NTRT_INT, "^(int|interface)( ([^[:space:]]+) (up|down))$" )   ///< mpt int[erface] IF {up|down}
	CLI_COMMAND( NTRT_ADDR, "^(addr|address)( (add|del) ("REGEX_IPV4"|"REGEX_IPV6")(/([[:digit:]]+))? dev ([[:alnum:]]+)|.*)$" )   ///< mpt addr[ess] {add|del} IP[/PREFIX] dev IF
	CLI_COMMAND( NTRT_RELOAD, "^reload(| ([^ ]+)| .*)$" )    ///< mpt reload [FILENAME]
	CLI_COMMAND( NTRT_DELETE, "^delete( ([^ ]+)|.*)$" )      ///< mpt delete FILENAME
	CLI_COMMAND( NTRT_SAVE, "^save(| ([^ ]+)| .*)$" )        ///< mpt save [FILENAME]
	CLI_COMMAND( NTRT_PATH, "^path( (up|down) ("REGEX_IPV4"|"REGEX_IPV6") ("REGEX_IPV4"|"REGEX_IPV6")| .*|)$" )    ///< mpt path {up|down} SRC_IP DST_IP
	CLI_COMMAND( NTRT_STATUS, "^show status$" )              ///< mpt show status
#undef CLI_COMMAND
    NTRT_NOCMD                   ///< None of the commands above was matched
} CMDTYPE;

/**
 * @defgroup MptHelp Predefined help outputs
 * @{
 */

/**
 * Help page of command mpt address
 */
#define HELP_ADDR "mpt addr[ess] {add | del} IP_ADDRESS[/PREF_LEN] dev INTERFACE\n\
\n\
IP_ADDRESS: The IP address (can be v4 or v6) to manipulate\n\
PREF_LEN:   The prefix length of the manipulated address\n\
                Default prefix length: 24 for IPv4, 64 for IPv6.\n\
INTERFACE:  The name of the interface related to the manipulated address\n\n"

/**
 * Help page of command mpt interface
 */
#define HELP_INT "mpt int[erface] INTEFACE {up | down}\n\
\n\
INTEFACE: The name of the interface e.g. eth0\n\n"

/**
 * Help page of command mpt reload
 */
#define HELP_RELOAD "mpt reload [FILENAME]\n\
\n\
FILENAME:   The file you have recently edited and needed to reload (Optional)\n\n"

/**
 * Help page of command mpt delete
 */
#define HELP_DELETE "mpt delete FILENAME\n\
\n\
FILENAME:   The file that contains connections no longer needed\n\n"

/**
 * Help page of command mpt save
 */
#define HELP_SAVE "mpt save [FILENAME]\n\
\n\
FILENAME:   Save changed connection informations to config file (Optional)\n\n"

/**
 * Help page of command mpt path
 */
#define HELP_PATH "mpt path {up | down} SRC_IP DST_IP\n\
\n\
up|down:    Change path status to up or down\n\
SRC_IP:     IP address of local endpoint\n\
DST_IP:     IP address of remote peer\n\n"

/**
 * Help page of command mpt path
 */
#define HELP_STATUS "mpt show status\n\n"

/**
 * Detailed help of all usable mpt commands
 */
#define HELP HELP_ADDR HELP_INT HELP_RELOAD HELP_DELETE HELP_SAVE HELP_PATH HELP_STATUS

/** @} */

/// Structure to save arguments of command mpt interface up|down
struct mpt_int {
    char * interface;           ///< Interface name
    char * mark;                ///< Mark interface up or down
};

/// Structure to save arguments of command mpt address add|del
struct mpt_addr {
    char * op;                  ///< Operator can be add or del
    char * ip;                  ///< IP Address
    char * mask;                ///< Network mask
    char * dev;                 ///< Interface name
};

/// Structure to save arguments of command mpt path up|down
struct mpt_change_path {
    char * op;                  ///< Operator can be up or down
    uint32_t local_ip[4];         ///< Local IP address
    uint32_t remote_ip[4];        ///< IP address of remote peer
};

#endif /* INCGUARD_ETC_CLI_H_ */
