#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

#include <libnl3/netlink/netlink.h>
#include <libnl3/netlink/socket.h>
#include <libnl3/netlink/genl/genl.h>
#include <libnl3/netlink/genl/ctrl.h>

#include "if_match.h"
#include "matchlib.h"
#include "matchlib_nl.h"

int main(void)
{
	struct nl_sock *nsd;
	uint32_t pid, min = 0, max = 5;
	int family, i;
	struct net_mat_hdr_node *hdr_gph_node;
	struct net_mat_tbl_node *tbl_gph_node;
	struct net_mat_hdr *headers;
	struct net_mat_action *actions;
	struct net_mat_tbl *tables;
	struct net_mat_port *ports;
	uint32_t phys_port_id = 0;
	uint32_t glort = 0, lport = 0;
	uint8_t bus, device, function;
	int err;

	nsd = match_nl_get_socket();
	if (!nsd) {
		fprintf(stderr, "Error: socket allocation failed\n");
		return -EINVAL;
	}

	pid = match_pid_lookup();
	if (pid == 0)
		return -EINVAL;

	family = NET_MAT_DFLT_FAMILY;

	fprintf(stderr, "%s: pid %i family %i\n", __func__, pid, family);
	printf("----------------------------------------\n");
	printf("Header Graph:\n");
	printf("----------------------------------------\n");
	headers = match_nl_get_headers(nsd, pid, 0, family);
	if (!headers) {
		fprintf(stderr, "Error: get_headers failed\n");
		return -EINVAL;
	}
	hdr_gph_node = match_nl_get_hdr_graph(nsd, pid, 0, family);
	if (!hdr_gph_node) {
		fprintf(stderr, "Error: get_header_graph failed\n");
		free(headers);
		return -EINVAL;
	}
	pp_header_graph(stdout, true, hdr_gph_node);
	free(hdr_gph_node);
	free(headers);
	printf("\n");

	printf("----------------------------------------\n");
	printf("Table Graph:\n");
	printf("----------------------------------------\n");
	actions = match_nl_get_actions(nsd, pid, 0, family);
	if (!actions) {
		fprintf(stderr, "Error: get_actions failed\n");
		return -EINVAL;
	}
	match_push_actions_ary(actions);
	tables = match_nl_get_tables(nsd, pid, 0, family);
	if (!tables) {
		fprintf(stderr, "Error: get_tables failed\n");
		free(actions);
		return -EINVAL;
	}
	match_push_tables_a(tables);
	tbl_gph_node = match_nl_get_tbl_graph(nsd, pid, 0, family);
	if (!tbl_gph_node) {
		fprintf(stderr, "Error: get_table_graph failed\n");
		free(tables);
		free(actions);
		return -EINVAL;
	}
	pp_table_graph(stdout, true, tbl_gph_node);
	free(tbl_gph_node);
	free(tables);
	free(actions);
	printf("\n");

	printf("----------------------------------------\n");
	printf("Headers:\n");
	printf("----------------------------------------\n");
	headers = match_nl_get_headers(nsd, pid, 0, family);
	if (!headers) {
		fprintf(stderr, "Error: get_headers failed\n");
		return -EINVAL;
	}
	for (i = 0 ; headers[i].uid ; i++)
		pp_header(stdout, true, &headers[i]);
	free(headers);
	printf("\n");

	printf("----------------------------------------\n");
	printf("Actions:\n");
	printf("----------------------------------------\n");
	actions = match_nl_get_actions(nsd, pid, 0, family);
	if (!actions) {
		fprintf(stderr, "Error: get_actions failed\n");
		return -EINVAL;
	}
	pp_actions(stdout, true, actions);
	free(actions);
	printf("\n");

	printf("----------------------------------------\n");
	printf("Tables:\n");
	printf("----------------------------------------\n");
	headers = match_nl_get_headers(nsd, pid, 0, family);
	if (!headers) {
		fprintf(stderr, "Error: get_headers failed\n");
		return -EINVAL;
	}
	hdr_gph_node = match_nl_get_hdr_graph(nsd, pid, 0, family);
	if (!hdr_gph_node) {
		fprintf(stderr, "Error: get_header_graph failed\n");
		free(headers);
		return -EINVAL;
	}
	actions = match_nl_get_actions(nsd, pid, 0, family);
	if (!actions) {
		fprintf(stderr, "Error: get_actions failed\n");
		free(hdr_gph_node);
		free(headers);
		return -EINVAL;
	}
	match_push_actions_ary(actions);
	tables = match_nl_get_tables(nsd, pid, 0, family);
	if (!tables) {
		fprintf(stderr, "Error: get_tables failed\n");
		free(actions);
		free(hdr_gph_node);
		free(headers);
		return -EINVAL;
	}
	for (i = 0 ; tables[i].uid ; i++)
		pp_table(stdout, true, &tables[i]);
	free(tables);
	free(actions);
	free(hdr_gph_node);
	free(headers);
	printf("\n");

	printf("----------------------------------------\n");
	printf("Ports:\n");
	printf("----------------------------------------\n");
	ports = match_nl_get_ports(nsd, pid, 0, family, min, max);
	if (!ports) {
		fprintf(stderr, "Error: get_ports failed\n");
		return -EINVAL;
	}
	pp_ports(stdout, true, ports);
	free(ports);
	printf("\n");

	printf("----------------------------------------\n");
	printf("pci to Logical Port Map:\n");
	printf("----------------------------------------\n");
	bus = 6;
	device = 0;
	function = 0;

	err = match_nl_pci_lport(nsd, pid, 0, family, bus, device, function,
			&lport, &glort);

	if (err) {
		fprintf(stderr, "Error: match_nl_pci_lport failed\n");
		return -EINVAL;
	} else
		printf("pci BUS:DEVICE.FUNCTION %02x:%02x.%d is logical port %d\n",
				bus, device, function, lport);

	printf("\n");

	printf("----------------------------------------\n");
	printf("Logical Port to Physical Port Map:\n");
	printf("----------------------------------------\n");
	err = match_nl_lport_to_phys_port(nsd, pid, 0, family, lport,
			&phys_port_id, &glort);
	if (err) {
		fprintf(stderr, "Error: lport_to_phys_port failed\n");
		return -EINVAL;
	} else
		printf("logical port %d is physical port %d\n", lport, phys_port_id);
	printf("\n");

	return 0;
}
