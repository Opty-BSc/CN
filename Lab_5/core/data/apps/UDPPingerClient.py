import sys
import time
from socket import *

# Get the server hostname and port as command line arguments
argv = sys.argv
host = argv[1]
port = argv[2]
timeout = 1  # in second

# Create UDP client socket
# Note the use of SOCK_DGRAM for UDP datagram packet
# Fill in start
clientSocket = socket(AF_INET, SOCK_DGRAM)
# Fill in end
# Set socket timeout as 1 second
# Fill in start
clientSocket.settimeout(1)
# Fill in end
# Command line argument is a string, change the port into integer
port = int(port)
# Sequence number of the ping message
ptime = 0

# Ping for 10 times
while ptime < 10:
	ptime += 1
	# Format the message to be sent
	data = "Ping " + str(ptime) + " " + time.asctime()

	try:
		# Sent time
		RTTb = time.time()
		# Send the UDP packet with the ping message
		# Fill in start
		clientSocket.sendto(data.encode(), (host, port))
		# Fill in end
		# Receive the server response
		# Fill in start
		message, address = clientSocket.recvfrom(1024)
		# Fill in end
		# Received time
		RTTa = time.time()
		# Display the server response as an output
		print("Reply from " + address[0] + ": " + message.decode())
		# Round trip time is the difference between sent and received time
		# Fill in start
		print("Ping {} {} ".format(ptime, RTTa - RTTb))
		# Fill in end
	except:
		# Server does not respond
		# Assume the packet is lost
		# Fill in start
		print("Requested timed out")
		# Fill in end
		continue

# Close the client socket
clientSocket.close()
