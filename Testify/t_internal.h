
#define TESTIFY_MINIMUM_WRITE_SPACE 1024

typedef struct{
	unsigned int ip;
	unsigned short port;
	int			socket;
	THandleType type;
	uint8		*read_buffer;
	uint		read_buffer_used;
	uint		read_buffer_pos;
	uint		read_buffer_size;
	uint		read_marker;
	uint8		*write_buffer;
	uint		write_buffer_pos;
	uint		write_buffer_size;
	void		*file;
	void		*text_copy;
	char		*file_name;
	boolean		debug_descriptor;
	boolean		debug_header;
	boolean		connected;
}THandle;

extern uint testify_unpack_buffer_get(THandle *handle);
extern boolean testify_pack_buffer_clear(THandle *handle);
extern void testify_handle_clear(THandle *handle, uint type);
extern uint testify_network_stream_receve(THandle *handle);