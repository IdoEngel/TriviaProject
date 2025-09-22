import socket
import TriviaClient_funcs
import json
import struct

def main():
    host = 'localhost'

    #get server port (the one we connect to)
    server_port = input("enter server port to connect to: ")
    try:
        server_port = int(server_port)
    except:
        print("port must be a number")
        return

    #bind client to random port in valid range
    sock = TriviaClient_funcs.bind_to_random_socket(1024, 65535)

    try:
        #connect to the server
        sock.connect((host, server_port))
 
        message = {
            "username": "IdoEngel",
            "password": "III12345"
        }
        json_message = json.dumps(message)

        my_bytes = bytearray()
        my_bytes.append(11)
        length_bytes = len(json_message).to_bytes(4, byteorder='big')
        my_bytes.extend(length_bytes)

        for _ in range(5):
            sock.sendall(my_bytes + json_message.encode('utf-8'))

            print(f'Server msg - \n{sock.recv(1024).decode()}')

    except Exception as e:
        print("something went wrong:", e)

    finally:
        sock.close()

if __name__ == '__main__':
    main()