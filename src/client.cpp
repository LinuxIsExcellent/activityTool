#include "client.h"

Client::Client(struct sockaddr_in & address, int fd)
{
	m_client_address = address;
	m_fd = fd;
}
