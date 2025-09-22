import socket

currPort = 49154

def bind_to_random_socket(low, up, max_tries=10):
    """
    @brief this function tries to creates a socket with a random port withing a givin range if port is avaible
    @param low the min value of gthe random port
    @param lup the max value of gthe random port
    @param max the max num of tries to find an avaible port in given range
    @returns sock the socket it created
    @throws exit(1) if max tries exceeded (just prints an error and exits for now - havnt fully implnted exeptions yet)
    """
    global currPort

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    port = 55560
    currPort += 1
    sock.bind(("localhost", port))

    print(f"client bound to port: {port}")
    return sock
        
