#ifndef CONNECTION_H
#define CONNECTION_H 

typedef enum {
	READING_LENTH,
	READING_PAYLOAD
} connection_phase_t;

typedef struct {
	int fd;
	connection_phase_t phase;
} connection_t;

#endif // CONNECTION_H
