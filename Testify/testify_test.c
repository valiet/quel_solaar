#include <stdio.h>
#include "testify.h"

//#define TESTIFY_TEST_FILE
//#define TESTIFY_TEST_UDP_SENDER
//#define TESTIFY_TEST_UDP_LISTENER
#define TESTIFY_TEST_TCP_SERVER
//#define TESTIFY_TEST_TCP_CLIENT

#ifdef TESTIFY_TEST_FILE

uint main()
{
	THandle *handle;
	uint i;
	/* save a file */
	handle = testify_file_save("my_file.bin");
	testify_debug_mode_set(handle, TRUE, "debug_file.txt");
	for(i = 0; i < 10; i++)
	{
		testify_pack_uint8(handle, i % 127, "VALUE A");
		testify_pack_int8(handle, i % 127, "VALUE B");
		testify_pack_uint16(handle, i % 1334, "VALUE C");
		testify_pack_int16(handle, i % 1234, "VALUE D");
		testify_pack_uint32(handle, i % 127, "VALUE E");
		testify_pack_int32(handle, i % 127, "VALUE F");
		testify_pack_real32(handle, (float)(i % 127) / 13.7, "VALUE G");
		testify_pack_real64(handle, (float)(i % 127) / 23.7, "VALUE H");
	}

	testify_free(handle);

	/*load the file */
	handle = testify_file_load("my_file.bin");
	testify_debug_mode_set(handle, TRUE, NULL);
	for(i = 0; i < 10; i++)
	{
		printf("[%u]\n", i);
		printf("loading uint8 A %u\n", (uint)testify_unpack_uint8(handle, "VALUE A"));
		printf("loading int8 B %u\n", (uint)testify_unpack_int8(handle, "VALUE B"));
		printf("loading uint16 C %u\n", (uint)testify_unpack_uint16(handle, "VALUE C"));
		printf("loading int16 D %u\n", (uint)testify_unpack_int16(handle, "VALUE D"));
		printf("loading uint32 E %u\n", (uint)testify_unpack_uint32(handle, "VALUE E"));
		printf("loading int32 F %u\n", (uint)testify_unpack_int32(handle, "VALUE F"));
		printf("loading real32 G %f\n", testify_unpack_real32(handle, "VALUE G"));
		printf("loading real64 H %f\n", testify_unpack_real64(handle, "VALUE H"));
	}
/*	printf("loading uint8 A %u\n", (uint)testify_unpack_uint8(handle, "VALUE A"));
	printf("loading int8 B %u\n", (uint)testify_unpack_int8(handle, "VALUE B"));
	printf("loading uint16 C %u\n", (uint)testify_unpack_uint16(handle, "VALUE C"));
	printf("loading uint32 E %u\n", (uint)testify_unpack_uint32(handle, "VALUE E"));
	printf("loading int16 D %u\n", (uint)testify_unpack_int16(handle, "VALUE D"));
	printf("loading int32 F %u\n", (uint)testify_unpack_int32(handle, "VALUE F"));
	printf("loading real32 G %f\n", testify_unpack_real32(handle, "VALUE G"));
	printf("loading real64 H %f\n", testify_unpack_real64(handle, "VALUE H"));	*/
	testify_free(handle);
}
#endif

#ifdef TESTIFY_TEST_UDP_SENDER

uint main()
{
	THandle *h;
	UnravelAddress address;
	h = testify_network_datagram_create(5000);
	testify_network_address_lookup(&address, "localhost");
	address.port = 4000;
	while(TRUE)
	{  
		testify_pack_uint8(h, 5, "value");
		testify_network_datagram_send(h, &address);
	}
}

#endif

#ifdef TESTIFY_TEST_UDP_LISTENER

uint main()
{
	boolean read;
	THandle *h;
	UnravelAddress address;
	h = testify_network_datagram_create(4000);
	while(TRUE)
	{  
		read = TRUE;
		testify_network_wait(&h, &read, NULL, 1, 1000000);
		if(read)
		{
			uint length;
			length = testify_network_receive(h, &address);
			if(length != 0)
				printf("Receving %u %u\n", length, testify_unpack_uint8(h, "value"));
		}
	}
}

#endif



extern THandle *testify_network_stream_address_create(const char *host_name, uint16 port);
extern void		testify_network_stream_address_destroy(THandle *handle);
extern int		testify_network_stream_send_force(THandle *handle);
extern THandle *testify_network_stream_wait_for_connection(THandle *listen);

extern THandle *testify_network_datagram_create(uint16 port);
extern int		testify_network_datagram_send(THandle *handle, UnravelAddress *to);

extern uint		testify_network_wait(THandle **handles, boolean *read, boolean *write, unsigned int handle_count, unsigned int microseconds);
extern int		testify_network_receive(THandle *handle, UnravelAddress *from);

#define TESTIFY_TEST_TCP_SERVER

#ifdef TESTIFY_TEST_TCP_SERVER

uint main()
{
	boolean read;
	THandle *h;
	UnravelAddress address;
	h = testify_network_stream_address_create(NULL, "localhost", 4000);
	while(TRUE)
	{  
		read = TRUE;
		testify_network_wait(&h, &read, NULL, 1, 1000000);
		if(read)
		{
			uint length;
			length = testify_network_receive(h, &address);
			if(length != 0)
				printf("Receving %u %u\n", length, testify_unpack_uint8(h, "value"));
		}
	}
}

#endif