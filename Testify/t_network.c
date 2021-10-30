/*
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "forge.h"

#if defined _WIN32
//#include <winsock.h>
#include <winsock2.h>
//#include <Iptypes.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <stdio.h>
typedef unsigned int uint;
typedef SOCKET VSocket;
#else
typedef int VSocket;
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#endif


#include <stdio.h>
#include <sys/types.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;
typedef char boolean;

#define	TRUE	1
#define	FALSE	0

#include "testify.h"


uint32 testify_socket_create(boolean stream, uint16 port)
{
	static boolean initialized = FALSE;
	struct sockaddr_in address;
    int buffer_size = 1 << 20;
	uint32 s;
#if defined _WIN32
	if(!initialized)
	{
		WSADATA wsaData;
		if(WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		{
			fprintf(stderr, "WSAStartup failed.\n");
			exit(1);
		}
	}
#endif
	initialized = TRUE;

	if(stream)
	{
		if((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
			return -1;	
	}else
	{
		if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			return -1;	
	}

#if defined _WIN32
	if(!stream)
	{
		unsigned long	one = 1UL;
		if(ioctlsocket(s, FIONBIO, &one) != 0)
			return -1;
	}
#else
	if(fcntl(s, F_SETFL, O_NONBLOCK) != 0)
	{
		fprintf(stderr, "v_network: Couldn't make socket non-blocking\n");
		exit(0); /* FIX ME */
		return -1;
	}
#endif
//	return s;
	if(port != 0 || !stream)
	{

		memset(&address, 0, sizeof(struct sockaddr));   /* Zero out structure */
		address.sin_family = AF_INET;     /* host byte order */
		address.sin_port = htons(port); /* short, network byte order */
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		if(bind(s, (struct sockaddr *) &address, sizeof(struct sockaddr)) != 0)
		{
			fprintf(stderr, "v_network: Failed to bind(), code %d (%s)\n", errno, strerror(errno));
			address.sin_family = AF_INET;     /* host byte order */
			address.sin_port = htons(0); /* short, network byte order */
			address.sin_addr.s_addr = INADDR_ANY;
			if(bind(s, (struct sockaddr *) &address, sizeof(struct sockaddr)) != 0)
			{
				fprintf(stderr, "v_network: Failed to bind(), code %d (%s)\n", errno, strerror(errno));
				exit(0); /* FIX ME */
			}
		}
		if(stream)
		   if(listen(s, 5) < 0)
				fprintf(stderr, "v_network: Failed to bind(), code %d (%s)\n", errno, strerror(errno));
	}
	if(setsockopt(s, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof buffer_size) != 0)
		fprintf(stderr, "Unravel: Couldn't set send buffer size of socket to %d\n", buffer_size);
	if(setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof buffer_size) != 0)
		fprintf(stderr, "Unravel: Couldn't set receive buffer size of socket to %d\n", buffer_size);
	return s;
}


void testify_socket_destroy(uint32 socket)
{
#if defined _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

boolean testify_network_address_lookup(UnravelAddress *address, char *dns_name, uint16 default_port)
{
	struct hostent *he;
	char *colon = NULL, *buf = NULL;
	address->port = default_port;
	if((colon = strchr(dns_name, ':')) != NULL)
	{
		size_t	hl = strlen(dns_name);
		if((buf = malloc(hl + 1)) != NULL)
		{
			unsigned int	tp;
			strcpy(buf, dns_name);
			colon = buf + (colon - dns_name);
			*colon = '\0';
			dns_name = buf;
			if(sscanf(colon + 1, "%u", &tp) == 1)
			{
				address->port = (unsigned short) tp;
				if(address->port != tp)
					return FALSE;
			}else
				return FALSE;
		}else
			return FALSE;
	}
	if(dns_name != NULL && (he = gethostbyname(dns_name)) != NULL)
	{
		memcpy(&address->ip, he->h_addr_list[0], he->h_length);
		address->ip = ntohl(address->ip);
		return TRUE;
	}
	return FALSE;
}


boolean testify_send_port_mapping(boolean udp, uint16 *port, uint *time)
{
	UnravelAddress to, from;
	THandle *h;
	uint i, size;
	char gateway[4 * 4];
	ULONG outBufLen;
	fd_set fd_read;
	struct timeval	tv;
	PIP_ADAPTER_ADDRESSES AdapterAddresses, a;

	outBufLen = 15000;
	AdapterAddresses = HeapAlloc(GetProcessHeap(), 0, outBufLen);
	GetAdaptersAddresses(AF_INET, /*AF_INET6*/ GAA_FLAG_INCLUDE_GATEWAYS, NULL, AdapterAddresses, &outBufLen);
	gateway[0] = 0;
	for(a = AdapterAddresses; a != NULL; a = a->Next)
	{
		uint8 *address;
		printf("FirstGatewayAddress %s %p\n", a->FriendlyName, a->FirstGatewayAddress);
		if(a->FirstUnicastAddress != NULL)
		{
			address = (uint8 *)a->FirstUnicastAddress->Address.lpSockaddr->sa_data;
			printf("Address %u.%u.%u.%u\n", (uint)address[2], (uint)address[3], (uint)address[4], (uint)address[5]);
		
			if(address[2] == 192 && address[3] == 168) /* this is a local address */
			{
				if(a->FirstGatewayAddress != NULL)
				{
					address = (uint8 *)a->FirstGatewayAddress->Address.lpSockaddr->sa_data;
					sprintf(gateway, "%u.%u.%u.%u", (uint)address[2], (uint)address[3], (uint)address[4], (uint)address[5]);
				}
				h = testify_network_datagram_create(2888);
				testify_network_address_lookup(&to, gateway, 5351);
				testify_pack_uint8(h, 0, "version");
				if(udp)
					testify_pack_uint8(h, 1, "UDP Protocol");
				else
					testify_pack_uint8(h, 1, "TCP Protocol");
				testify_pack_uint16(h, 0, "Reserved");
				testify_pack_uint16(h, *port, "Internal Port");
				testify_pack_uint16(h, *port, "External Port");
				testify_pack_uint32(h, 60, "time");
				testify_network_datagram_send(h, &to);
				FD_ZERO(&fd_read);
				FD_SET(h->socket, &fd_read);
				tv.tv_sec  = 1;
				tv.tv_usec = 0;
				select(h->socket + 1, &fd_read, NULL, NULL, &tv);
				if(size = testify_network_receive(h, &from) >= 16)
				{

					testify_unpack_uint8(h, "version");
					testify_unpack_uint8(h, "opcode");
					testify_unpack_uint16(h, "result code");
					testify_unpack_uint32(h, "time");
					testify_unpack_uint16(h, "private port");
					*port = testify_unpack_uint16(h, "public port");
					*time = testify_unpack_uint32(h, "lifetime");
					printf("got");
				}
				testify_free(h);
			}
		}
	}
	HeapFree(GetProcessHeap(), 0, AdapterAddresses);

}

THandle *testify_network_stream_address_create(const char *host_name, uint16 port)
{
	THandle *handle;
	struct hostent *he;
	char *colon = NULL, *buf = NULL;
	boolean ok = FALSE;
	uint socket;
	if(host_name == NULL)
	{
		uint time = 60 * 60 * 2;
		socket = testify_socket_create(TRUE, port);
		testify_send_port_mapping(FALSE, &port, &time);
	}else		
		socket = testify_socket_create(TRUE, 0);
/*	if(host_name == NULL)
		if(listen(address->socket, 16) < 0)
	        printf("listen() failed");*/
	if(host_name != NULL)
	{
		UnravelAddress ip;
		struct sockaddr_in address_in;
		int a;
		if(testify_network_address_lookup(&ip, host_name, port))
		{
			memset(&address_in, 0, sizeof(address_in));
			address_in.sin_family = AF_INET;  
			address_in.sin_port = htons(ip.port); 
			address_in.sin_addr.s_addr = ntohl(ip.ip);
			a = connect(socket, &address_in, sizeof(address_in));
			if(a < 0) 
			{
				printf("no connect\n");
				return NULL;
			}
			printf("port %u\n", (uint)ip.port);
		}else
			return NULL;
	}
#if defined _WIN32
	{
		unsigned long	one = 1UL;
		if(ioctlsocket(socket, FIONBIO, &one) != 0)
			return NULL;
	}
#else
	if(fcntl(socket, F_SETFL, O_NONBLOCK) != 0)
	{
		fprintf(stderr, "v_network: Couldn't make socket non-blocking\n");
		exit(0); /* FIX ME */
		return NULL;
	}
#endif
	handle = malloc(sizeof *handle);
	if(host_name == NULL)
		testify_handle_clear(handle, T_HT_STREAMING_SERVER);
	else
		testify_handle_clear(handle, T_HT_STREAMING_CONNECTION);
	handle->port = port;
	handle->socket = socket;
	handle->connected = TRUE;
	if(host_name == NULL)
	{
		uint time = 60 * 60 * 2;
		testify_send_port_mapping(FALSE, &port, &time);
	}
	return handle;
}

void testify_network_stream_address_destroy(THandle *handle)
{
	if(handle->read_buffer != NULL)
		free(handle->read_buffer);
	if(handle->write_buffer != NULL)
		free(handle->write_buffer);
	if(handle->file != NULL)
		fclose(handle->file);
	if(handle->text_copy != NULL)
		fclose(handle->text_copy);
}

THandle *testify_network_stream_wait_for_connection(THandle *listener, UnravelAddress *from)
{
	THandle *handle;
	struct sockaddr_in address;
	int socket;
	uint length;
	boolean read = TRUE;
	if(listen(listener->socket, 16) >= 0)
	{
		testify_network_wait(&listener, &read, NULL, 1, 1);
		if(read)
		{
			length = sizeof(struct sockaddr_in);
			if((socket = accept(listener->socket, (struct sockaddr *) &address, &length)) >= 0)
			{

#if defined _WIN32
//		if(!stream)
		{
			unsigned long	one = 1UL;
			if(ioctlsocket(socket, FIONBIO, &one) != 0)
				return NULL;
		}
#else
		if(fcntl(socket, F_SETFL, O_NONBLOCK) != 0)
		{
			fprintf(stderr, "v_network: Couldn't make socket non-blocking\n");
			exit(0); /* FIX ME */
			return NULL;
		}
#endif

				handle = malloc(sizeof *handle);
				testify_handle_clear(handle, T_HT_STREAMING_CONNECTION);
				handle->debug_descriptor = listener->debug_descriptor;
				handle->socket = socket;
				handle->ip = ntohl(address.sin_addr.s_addr);
				handle->port = ntohs(address.sin_port);
				if(from != NULL)
				{
					from->ip = handle->ip;
					from->port = handle->port;
				}	
				return handle;
			}
		}
	}else
	{
		handle->connected = FALSE;
		printf("UNRAVEL; Listerner failed\n");
	}
	return NULL;
}
/*
typedef struct{
	unsigned int ip;
	unsigned short port;
	int			socket;
	THandleType type;
	uint8		*read_buffer;
	uint		read_buffer_size;
	uint		read_buffer_pos;	
	uint8		*write_buffer;
	uint		write_buffer_size;
	void		*file;
	void		*text_copy;
	boolean		debug_descriptor;
}THandle;*/


THandle *testify_network_datagram_create(uint16 port)
{
	THandle *handle;
	handle = malloc(sizeof *handle);
	handle->ip = 0;
	handle->port = port;
	handle->socket = testify_socket_create(FALSE, port);
	handle->type = T_HT_PACKET_PEER;
	handle->read_buffer = malloc((sizeof *handle->read_buffer) * 1500);
	handle->read_buffer_pos = 0;	
	handle->read_buffer_size = 1500;
	handle->write_buffer = malloc((sizeof *handle->write_buffer) * 1500);
	handle->write_buffer_pos = 0;
	handle->write_buffer_size = 1500;
	handle->file = NULL;
	handle->text_copy = NULL;
	handle->debug_descriptor = FALSE;
	return handle;
}

int testify_network_datagram_send(THandle *handle, UnravelAddress *to)
{
	struct sockaddr_in	address_in;
	int out;
	address_in.sin_family = AF_INET;
	address_in.sin_port = htons(to->port);
	address_in.sin_addr.s_addr = htonl(to->ip);
	memset(&address_in.sin_zero, 0, sizeof address_in.sin_zero);
	out = sendto(handle->socket, handle->write_buffer, handle->write_buffer_pos, 0, (struct sockaddr *)&address_in, sizeof(struct sockaddr_in));
	if(out < 0)
		handle->connected = FALSE;
	handle->write_buffer_pos = 0;
	return out;
}

int testify_network_stream_send_force(THandle *handle)
{
	int i, size;
	if(handle->type == T_HT_STREAMING_CONNECTION)
	{
		if(handle->write_buffer_pos != 0 && handle->connected)
		{
			boolean write;
		/*	testify_network_wait(&handle, NULL, &write, 1, 0);
			if(!write)
				return FALSE;*/

			size = send(handle->socket, handle->write_buffer, handle->write_buffer_pos, 0);

			if(size > 0)
			{
				uint j = 0;
				for(i = 0; i < size; i++)
				{
					if(handle->write_buffer[i] == 253)
						j++;
					else
						j = 0;
					if(j > 20)
						j = 20;
				}
			}
			if(size == 0)
				size = 0;
			if(size < 0)
			{
				int error;
#if defined _WIN32
				error = WSAGetLastError();
				if(error == WSAEWOULDBLOCK)
					return 0;
#else
				error = errno;
				if(error ==  EAGAIN || error == EWOULDBLOCK || error == 0) /* Given that its non blocking recv may return an error if no data is available. EWOULDBLOCK */
					return 0;
#endif
				printf("send Error %u %s\n", error);
				handle->connected = FALSE;
				return -1;
			}
			if(size == handle->write_buffer_pos)
				handle->write_buffer_pos = 0;
			else if(size != 0)
			{
				for(i = 0; i < handle->write_buffer_pos - size; i++)
					handle->write_buffer[i] = handle->write_buffer[size + i];
				handle->write_buffer_pos -= size;
			}
			return size;
		}
	}else
		printf("testify_network_stream_send_force FAILED %u %u %u", handle->type, T_HT_STREAMING_CONNECTION, handle->connected);

	if(handle->type == T_HT_FILE_WRITE)
		testify_pack_buffer_clear(handle);
	return 0;
}


uint testify_network_wait(THandle **handles, boolean *read, boolean *write, unsigned int handle_count, unsigned int microseconds)
{
	struct timeval	tv;
	fd_set fd_read, fd_write;
	unsigned int i, max = 0;

/*	if(microseconds == 0)
		return 0;*/
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	for(i = 0; i < handle_count; i++)
	{
		if(read != NULL && read[i] && handles[i]->connected)
		{
			FD_SET(handles[i]->socket, &fd_read);
			if(max < handles[i]->socket)
				max = handles[i]->socket;
		}
		if(write != NULL && write[i] && handles[i]->connected)
		{
			FD_SET(handles[i]->socket, &fd_write);
			if(max < handles[i]->socket)
				max = handles[i]->socket;
		}
	}
	tv.tv_sec  = microseconds / 1000000;
	tv.tv_usec = microseconds % 1000000;
	select(max + 1, &fd_read, &fd_write, NULL, &tv);
	for(i = 0; i < handle_count; i++)
	{
		if(read != NULL && read[i] && handles[i]->connected)
			read[i] = FD_ISSET(handles[i]->socket, &fd_read);
		if(write != NULL && write[i] && handles[i]->connected)
			write[i] = FD_ISSET(handles[i]->socket, &fd_write);
	}
	return microseconds;
}


uint testify_network_stream_receve(THandle *handle)
{
	static uint seed = 0;
	boolean read = TRUE;
	int get;
/*	if(f_randf(seed++) < 0.8)
		return 0;*/
/*	testify_network_wait(&handle, &read, NULL, 0, 0);
	if(!read)
		return FALSE;*/
	get = recv(handle->socket, &handle->read_buffer[handle->read_buffer_used], handle->read_buffer_size - handle->read_buffer_used, 0);
//	get = recv(handle->socket, &handle->read_buffer[handle->read_buffer_used], 1, 0);

/*	if(get > 0)
	{
		uint i;
		printf("Network GET\n");
		for(i = 0; i < get; i++)
			printf("data[i] = %u %c\n", i, (uint)handle->read_buffer[handle->read_buffer_used + i], handle->read_buffer[handle->read_buffer_used + i]);
	}*/
	
	if(get < 0)
	{
		int error;
#if defined _WIN32
		error = WSAGetLastError();
		if(error == WSAEWOULDBLOCK)
			return 0;
#else
		error = errno;
		if(error ==  EAGAIN || error == EWOULDBLOCK || error == 0) /* Given that its non blocking recv may return an error if no data is available. EWOULDBLOCK */
			return 0;
#endif
		printf("recv Error %u\n", error);
		handle->connected = FALSE;
		return -1;
	}
	return get;
}

/*
typedef struct{
	unsigned int ip;
	unsigned short port;
	int			socket;
	THandleType type;
	uint8		*read_buffer;
	uint		read_buffer_size;
	uint		read_buffer_pos;	
	uint8		*write_buffer;
	uint		write_buffer_size;
	void		*file;
	void		*text_copy;
	boolean		debug_descriptor;
}THandle;
*/
int testify_network_receive(THandle *handle, UnravelAddress *from)
{
	struct	sockaddr_in address_in;
	int	from_length = sizeof address_in, len;
	memset(&address_in, 0, sizeof address_in);
	address_in.sin_family = AF_INET;
	address_in.sin_port = htons(handle->port); 
	address_in.sin_addr.s_addr = INADDR_ANY;
	len = recvfrom(handle->socket,  handle->read_buffer, 1500, 0, (struct sockaddr *) &address_in, &from_length);
	handle->read_buffer_pos = 0; 
	if(len > 0 && from != NULL)
	{
		handle->read_buffer_used = len;
		from->ip   = ntohl(address_in.sin_addr.s_addr);
		from->port = ntohs(address_in.sin_port);
		return len;
	}
	return 0;
}

void testify_network_address_compare(UnravelAddress *address, char *dns_name)
{
}


