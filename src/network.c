#include "kernel.h"
#include "vga.h"

// Network stack structures
typedef struct network_packet {
    u8 *data;
    u32 size;
    u32 protocol;
    struct network_packet *next;
} network_packet_t;

// Network interface structure
typedef struct network_interface {
    char name[16];
    u8 mac_address[6];
    u32 ip_address;
    u32 netmask;
    u32 gateway;
    u8 active;
} network_interface_t;

// Simple packet queue
static network_packet_t *packet_queue = NULL;
static network_interface_t network_interfaces[4];
static u32 interface_count = 0;

// Protocol definitions
#define PROTO_ARP  0x0806
#define PROTO_IP   0x0800
#define PROTO_ICMP 0x01
#define PROTO_TCP  0x06
#define PROTO_UDP  0x11

// Simple ARP table
typedef struct arp_entry {
    u32 ip_address;
    u8 mac_address[6];
    u32 timestamp;
    u8 valid;
} arp_entry_t;

static arp_entry_t arp_table[16];
static u32 arp_entries = 0;

// Initialize network stack
void network_init(void) {
    // Initialize interfaces
    memset(network_interfaces, 0, sizeof(network_interfaces));
    interface_count = 0;
    
    // Initialize ARP table
    memset(arp_table, 0, sizeof(arp_table));
    arp_entries = 0;
    
    // Create loopback interface
    strcpy(network_interfaces[0].name, "lo");
    network_interfaces[0].ip_address = 0x7F000001; // 127.0.0.1
    network_interfaces[0].netmask = 0xFF000000;    // 255.0.0.0
    network_interfaces[0].gateway = 0;
    network_interfaces[0].active = 1;
    memset(network_interfaces[0].mac_address, 0, 6);
    interface_count++;
    
    // Create dummy ethernet interface
    strcpy(network_interfaces[1].name, "eth0");
    network_interfaces[1].ip_address = 0xC0A80002; // 192.168.0.2
    network_interfaces[1].netmask = 0xFFFFFF00;    // 255.255.255.0
    network_interfaces[1].gateway = 0xC0A80001;    // 192.168.0.1
    network_interfaces[1].active = 1;
    // Dummy MAC address
    network_interfaces[1].mac_address[0] = 0x00;
    network_interfaces[1].mac_address[1] = 0x50;
    network_interfaces[1].mac_address[2] = 0x56;
    network_interfaces[1].mac_address[3] = 0xC0;
    network_interfaces[1].mac_address[4] = 0x00;
    network_interfaces[1].mac_address[5] = 0x02;
    interface_count++;
    
    vga_printf("Network stack initialized with %d interfaces\n", interface_count);
}

// Add packet to queue
static void network_queue_packet(u8 *data, u32 size, u32 protocol) {
    network_packet_t *packet = (network_packet_t *)kmalloc(sizeof(network_packet_t));
    if (!packet) return;
    
    packet->data = (u8 *)kmalloc(size);
    if (!packet->data) {
        kfree(packet);
        return;
    }
    
    memcpy(packet->data, data, size);
    packet->size = size;
    packet->protocol = protocol;
    packet->next = NULL;
    
    // Add to queue
    if (!packet_queue) {
        packet_queue = packet;
    } else {
        network_packet_t *last = packet_queue;
        while (last->next) last = last->next;
        last->next = packet;
    }
}

// Process next packet from queue
static void network_process_packet(void) {
    if (!packet_queue) return;
    
    network_packet_t *packet = packet_queue;
    packet_queue = packet->next;
    
    vga_printf("Processing network packet: protocol 0x%x, size %d bytes\n",
               packet->protocol, packet->size);
    
    // Simple packet processing based on protocol
    switch (packet->protocol) {
        case PROTO_ARP:
            vga_puts("  ARP packet received\n");
            break;
        case PROTO_IP:
            vga_puts("  IP packet received\n");
            break;
        default:
            vga_printf("  Unknown protocol: 0x%x\n", packet->protocol);
            break;
    }
    
    kfree(packet->data);
    kfree(packet);
}

// List network interfaces
void network_list_interfaces(void) {
    vga_printf("Network Interfaces:\n");
    vga_printf("Name\tIP Address\tMAC Address\t\tStatus\n");
    vga_printf("----\t----------\t-----------\t\t------\n");
    
    for (u32 i = 0; i < interface_count; i++) {
        u32 ip = network_interfaces[i].ip_address;
        vga_printf("%s\t%d.%d.%d.%d\t", 
                   network_interfaces[i].name,
                   (ip >> 24) & 0xFF,
                   (ip >> 16) & 0xFF,
                   (ip >> 8) & 0xFF,
                   ip & 0xFF);
        
        u8 *mac = network_interfaces[i].mac_address;
        vga_printf("%02x:%02x:%02x:%02x:%02x:%02x\t",
                   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
        vga_printf("%s\n", network_interfaces[i].active ? "UP" : "DOWN");
    }
}

// Send ping (ICMP echo request) - simplified
int network_ping(u32 target_ip) {
    vga_printf("PING %d.%d.%d.%d\n",
               (target_ip >> 24) & 0xFF,
               (target_ip >> 16) & 0xFF,
               (target_ip >> 8) & 0xFF,
               target_ip & 0xFF);
    
    // Simulate ping response
    vga_puts("64 bytes from target: icmp_seq=1 ttl=64 time=1.234 ms\n");
    vga_puts("--- ping statistics ---\n");
    vga_puts("1 packets transmitted, 1 received, 0% packet loss\n");
    
    return 0;
}

// Display network statistics
void network_stats(void) {
    vga_printf("Network Statistics:\n");
    vga_printf("  Interfaces: %d\n", interface_count);
    vga_printf("  ARP entries: %d\n", arp_entries);
    
    // Count packets in queue
    u32 queued_packets = 0;
    network_packet_t *packet = packet_queue;
    while (packet) {
        queued_packets++;
        packet = packet->next;
    }
    vga_printf("  Queued packets: %d\n", queued_packets);
}

// Simple socket interface placeholder
int network_socket(int domain, int type, int protocol) {
    (void)domain; (void)type; (void)protocol;
    vga_puts("Socket created (placeholder implementation)\n");
    return 1; // Return dummy socket descriptor
}

int network_bind(int sockfd, u32 addr, u16 port) {
    (void)sockfd;
    vga_printf("Socket bound to %d.%d.%d.%d:%d\n",
               (addr >> 24) & 0xFF,
               (addr >> 16) & 0xFF,
               (addr >> 8) & 0xFF,
               addr & 0xFF,
               port);
    return 0;
}

int network_listen(int sockfd, int backlog) {
    (void)sockfd; (void)backlog;
    vga_puts("Socket listening for connections\n");
    return 0;
}

// Simulate receiving a test packet
void network_test_receive(void) {
    u8 test_packet[] = {0x45, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x40, 0x00,
                        0x40, 0x01, 0x00, 0x00, 0xC0, 0xA8, 0x00, 0x01,
                        0xC0, 0xA8, 0x00, 0x02, 0x08, 0x00, 0xF7, 0xFC,
                        0x00, 0x00, 0x00, 0x00};
    
    vga_puts("Simulating network packet reception...\n");
    network_queue_packet(test_packet, sizeof(test_packet), PROTO_IP);
    network_process_packet();
}