/*
 * offline_shooter.c
 *
 *  Created on: 2017. 8. 18.
 *      Author: root
 */


#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "log.h"


void shooter_pcap_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet);

bool offline_shooter(const char *filename)
{

	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];

	pcap = pcap_open_offline(filename, errbuf);
	if (!pcap) {
		echo.e("pcap_open_offline - %s", errbuf);
		return false;
	}

	if (pcap_loop(pcap, 0, shooter_pcap_handler, NULL) < 0) {
		echo.e("pcap_loop - %s", pcap_geterr(pcap));
		pcap_close(pcap);
		return false;
	}

	pcap_close(pcap);
	return true;
}


void shooter_pcap_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	size_t caplen = pkthdr->caplen;
	size_t len = pkthdr->len;

	echo.i("caplen=%ld, len=%ld", caplen, len);
}


int main()
{
	offline_shooter("beacon.pcap");
}
