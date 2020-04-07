# Import socket module
from socket import *
import sys  # In order to terminate the program

# Create a TCP client socket
# (AF_INET is used for IPv4 protocols)
# (SOCK_STREAM is used for TCP)
clientSocket = socket(AF_INET, SOCK_STREAM)

# arguments of the client command
serverName = sys.argv[1]
serverPort = int(sys.argv[2])
filename = sys.argv[3]

# define the message to send
message = 'GET {} HTTP/1.1'.format(filename)

# Set up a new connection to the server
# Fill in start
clientSocket.connect((serverName, serverPort))
# Fill in end

# Send the message to the server
# Fill in start
clientSocket.send(message.encode())
# Fill in end

# Receive the response from the server
modifiedMessage = clientSocket.recv(1024)
response = ''

while len(modifiedMessage) > 0:
    response += modifiedMessage.decode()
    modifiedMessage = clientSocket.recv(1024)

print(response)

# Close the client socket
# Fill in start
clientSocket.close()
# Fill in end
sys.exit()  # Terminate the program
