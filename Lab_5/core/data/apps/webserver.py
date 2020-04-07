# Import socket module
from socket import *
import sys  # In order to terminate the program

# Create a TCP server socket
# (AF_INET is used for IPv4 protocols)
# (SOCK_STREAM is used for TCP)

serverSocket = socket(AF_INET, SOCK_STREAM)

# Assign a port number
serverPort = 6789

# Bind the socket to server address and server port
# Fill in start
serverSocket.bind(('', serverPort))
# Fill in end

# Listen to at most 1 connection at a time
# Fill in start
serverSocket.listen(1)
# Fill in end


def sendFile(conn, filename):
	f = open(filename)
	# Store the entire content of the requested file
	# in a temporary buffer
	outputdata = f.read()

	# Send the HTTP response header line
	# to the connection socket
	# Send the content of the requested file
	# to the connection socket
	for i in range(0, len(outputdata)):
		conn.send(outputdata[i].encode())
	conn.send("\r\n".encode())
	f.close()


# Server should be up and running and listening
# to the incoming connections
while True:
	print('The server is ready to receive')

	# Set up a new connection from the client
	# Fill in start
	conn, addr = serverSocket.accept()
	# Fill in end

	# If an exception occurs during the execution of try clause
	# the rest of the clause is skipped
	# If the exception type matches the word after except
	# the except clause is executed
	try:
		# Receives the request message from the client
		message = conn.recv(1024)
		# Extract path of requested object from message
		# The Path is the second part of HTTP header,
		# identif. by [1]
		filename = message.split()[1]
		# The extracted path of the HTTP request includes
		# a character '\', we read the path from
		# the second character

		# Send the HTTP response header line
		# to the connection socket
		# Fill in start
		conn.send("HTTP/1.1 200 OK\r\n".encode())
		# Fill in end
		sendFile(conn, filename[1:])

		# Close the client connection socket
		conn.close()

	except (IOError, IndexError, OSError):
		# Send HTTP response message for file not found
		conn.send("HTTP/1.1 404 Not Found\r\n".encode())

		# Fill in start
		sendFile(conn, "index.html")
		# Fill in end

		# Close the client connection socket
		# Fill in start
		conn.close()
		# Fill in end

serverSocket.close()
sys.exit()  # Terminate the program after sending the corresponding data
