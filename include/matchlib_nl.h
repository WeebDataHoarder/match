/*******************************************************************************

  MATCH Library - Helpers for working on MATCH Interface
  Author: John Fastabend <john.r.fastabend@intel.com>
  Copyright (c) <2015>, Intel Corporation

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _MATCHLIB_NL_H
#define _MATCHLIB_NL_H
#include "if_match.h"
#include "matstream.h"


void match_nl_set_verbose(int new_verbose);
void match_nl_set_streamer(struct mat_stream *streamer);

struct nl_sock *match_nl_get_socket(void);

uint32_t match_pid_lookup(void);

struct net_mat_hdr *match_nl_get_headers(struct nl_sock *nsd, uint32_t pid,
					 unsigned int ifindex, int family);
struct net_mat_action *match_nl_get_actions(struct nl_sock *nsd, uint32_t pid,
					    unsigned int ifindex, int family);
struct net_mat_tbl *match_nl_get_tables(struct nl_sock *nsd, uint32_t pid,
					unsigned int ifindex, int family);
struct net_mat_hdr_node *match_nl_get_hdr_graph(struct nl_sock *nsd,
						uint32_t pid,
						unsigned int ifindex,
						int family);
struct net_mat_tbl_node *match_nl_get_tbl_graph(struct nl_sock *nsd,
						uint32_t pid,
						unsigned int ifindex,
						int family);

int match_nl_set_del_rules(struct nl_sock *nsd, uint32_t pid,
		      unsigned int ifindex, int family,
		      struct net_mat_rule *rule, uint8_t cmd);
struct net_mat_rule *match_nl_get_rules(struct nl_sock *nsd, uint32_t pid,
                      unsigned int ifindex, int family,
                      uint32_t tableid, uint32_t min, uint32_t max);
int match_nl_set_port(struct nl_sock *nsd, uint32_t pid,
                      unsigned int ifindex, int family, struct net_mat_port *port);
struct net_mat_port *match_nl_get_ports(struct nl_sock *nsd, uint32_t pid,
                      unsigned int ifindex, int family, uint32_t min, uint32_t max);
int match_nl_create_update_destroy_table(struct nl_sock *nsd, uint32_t pid,
				unsigned int ifindex, int family,
				struct net_mat_tbl *table, uint8_t cmd);

uint32_t match_nl_find_header(struct net_mat_hdr *hdr,
			     struct net_mat_hdr *search);
uint32_t match_nl_find_action_by_name(char *name,
				     struct net_mat_action *action);
uint32_t match_nl_find_instance(struct net_mat_hdr_node *graph,
			       uint32_t uid, uint32_t next);
uint32_t match_nl_find_table_with_action(struct net_mat_tbl *tbls,
					uint32_t action, uint32_t next);

static inline int
match_nl_set_rules(struct nl_sock *nsd, uint32_t pid,
                     unsigned int ifindex, int family,
                     struct net_mat_rule *rule)
{
        return match_nl_set_del_rules(nsd, pid, ifindex, family, rule,
                                           NET_MAT_TABLE_CMD_SET_RULES);
}

static inline int
match_nl_del_rules(struct nl_sock *nsd, uint32_t pid,
                     unsigned int ifindex, int family,
                     struct net_mat_rule *rule)
{
        return match_nl_set_del_rules(nsd, pid, ifindex, family, rule,
                                           NET_MAT_TABLE_CMD_DEL_RULES);
}


static inline int
match_nl_update_table(struct nl_sock *nsd, uint32_t pid,
		     unsigned int ifindex, int family,
		     struct net_mat_tbl *table)
{
	return match_nl_create_update_destroy_table(nsd, pid, ifindex, family, table,
					   NET_MAT_TABLE_CMD_UPDATE_TABLE);
}

static inline int
match_nl_create_table(struct nl_sock *nsd, uint32_t pid,
		     unsigned int ifindex, int family,
		     struct net_mat_tbl *table)
{
	return match_nl_create_update_destroy_table(nsd, pid, ifindex, family, table,
					   NET_MAT_TABLE_CMD_CREATE_TABLE);
}

static inline int
match_nl_destroy_table(struct nl_sock *nsd, uint32_t pid,
                     unsigned int ifindex, int family,
                     struct net_mat_tbl *table)
{
        return match_nl_create_update_destroy_table(nsd, pid, ifindex, family, table,
                                           NET_MAT_TABLE_CMD_DESTROY_TABLE);
}

int match_nl_pci_lport(struct nl_sock *nsd, uint32_t pid,
		unsigned int ifindex, int family,
		uint8_t bus, uint8_t device, uint8_t function,
		uint32_t *lport, uint32_t *glort);

int match_nl_mac_lport(struct nl_sock *nsd, uint32_t pid,
		unsigned int ifindex, int family,
		uint64_t mac, uint32_t *lport,
		uint32_t *glort);

int match_nl_lport_to_phys_port(struct nl_sock *nsd, uint32_t pid,
		unsigned int ifindex, int family,
		uint32_t lport, uint32_t *phys_port,
		uint32_t *glort);

#endif
