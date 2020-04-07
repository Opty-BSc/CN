from socket import *
import os
import sys
import struct
import time
import select
import binascii

if sys.version_info[0] < 3:
    raise Exception("Python 3 or a more recent version is required.")

ICMP_ECHO_REQUEST = 8
MAX_HOPS = 30
TIMEOUT = 2.0
TRIES = 2
ID = os.getpid() & 0xffff
# Get arguments (destination hostname or IP address)
hostname = sys.argv[1]

# The packet that we shall send to each router along the path is the ICMP echo
# request packet, which is exactly what we had used in the ICMP Pinger
# We shall use the same packet that we built in the ICMP Pinger.
def checksum(string):
    # get the code for this function from the ICMP Pinger Lab
    # Fill in start
    csum = 0
    countTo = (len(string) // 2) * 2
    count = 0

    while count < countTo:
        thisVal = string[count + 1] * 256 + string[count]
        csum = csum + thisVal
        csum = csum & 0xffffffff
        count = count + 2

    if countTo < len(string):
        csum = csum + string[len(string) - 1]
        csum = csum & 0xffffffff

    # Now prepare the checksum value
    csum = (csum >> 16) + (csum & 0xffff)           # convolute first time
    csum = csum + (csum >> 16)                      # convolute second time
    answer = ~csum                                  # one's component
    answer = answer & 0xffff                        # get final checksum
    answer = answer >> 8 | (answer << 8 & 0xff00)   # Convert tp big ending
    return answer
    # Fill in end

# In this function we make the checksum of our packet
def build_packet():
    # In the sendOnePing() method of the ICMP Pinger,
    # firstly the header of the packet to be sent was made,
    # secondly the checksum was appended to the header and then
    # finally the complete packet was sent to the destination.
    # For the Tracerouter, make the header in a similar way to the ICMP Pinger.
    # Append checksum to the header.
    # Header is type (8), code (8), checksum (16), sequence (16)
    # Make a dummy header with a 0 checksum
    # struct -- Interpret strings as packed binary data
    
    # Fill in start
    myChecksum = 0
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    data = struct.pack("d", time.time())
    # Calculate the checksum on the data and the dummy header.
    # myChecksum = checksum(str(header + data))
    myChecksum = htons(checksum(header + data))
    # Get the right checksum
    if sys.platform == 'darwin':
        # Convert 16-bit integers from host to network byte order
        myChecksum &= 0xffff
    # Put the right checksum in the header.
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    # Fill in end
    
    # Do not send the packet yet, just return the final packet in this function.
    # So the function ending should look like this:
    packet = header + data
    return packet

def get_route(hostname):
    timeLeft = TIMEOUT
    # Resolve the hostname via DNS (or just use the IP Address) and display it.
    destAddr = gethostbyname(hostname)
    print("Route to " + hostname + " (" + destAddr + "): " )

    for ttl in range(1,MAX_HOPS):
        for tries in range(TRIES):
            destAddr = gethostbyname(hostname)
            # SOCK_RAW is a powerful socket type.
            # For more details:   http://sock-raw.org/papers/sock_raw
            # Make here a raw socket named mySocket, and bind it
            # Fill in start
            icmp = getprotobyname("icmp")
            mySocket = socket(AF_INET, SOCK_RAW, icmp)
            mySocket.bind(("", 0))
            # Fill in end

            # setsockopt method is used to set the time-to-live field.
            mySocket.setsockopt(IPPROTO_IP, IP_TTL, struct.pack('I', ttl))
            mySocket.settimeout(TIMEOUT)
            try:
                d = build_packet()
                mySocket.sendto(d, (hostname, 0))
                t = time.time()
                startedSelect = time.time()
                whatReady = select.select([mySocket], [], [], timeLeft)
                howLongInSelect = (time.time() - startedSelect)
                if whatReady[0] == []: # Timeout
                    print("  *        *        *    Request timed out.")
                recvPacket, addr = mySocket.recvfrom(1024)
                timeReceived = time.time()
                timeLeft = timeLeft - howLongInSelect
                if timeLeft <= 0:
                    print("  *        *        *    Request timed out.")

            except timeout:
                continue

            else:
                # Fetch the ICMP type and code from the received packet
                # Fill in start
                types = struct.unpack("b", recvPacket[20:21])[0]
                # Fill in end

                if types == 11:
                    bytes = struct.calcsize("d")
                    timeSent = struct.unpack("d", recvPacket[28:28 + bytes])[0]
                    print("  %d    rtt = %.0f ms    %s" %(ttl, (timeReceived -t)*1000, addr[0]))

                elif types == 3:
                    bytes = struct.calcsize("d")
                    timeSent = struct.unpack("d", recvPacket[28:28 + bytes])[0]
                    print("  %d    rtt = %.0f ms    %s" %(ttl, (timeReceived-t)*1000, addr[0]))

                elif types == 0:
                    bytes = struct.calcsize("d")
                    timeSent = struct.unpack("d", recvPacket[28:28 + bytes])[0]
                    print("  %d    rtt = %.0f ms    %s" %(ttl, (timeReceived - timeSent)*1000, addr[0]))
                    return

                else:
                    print("error")
                break

            finally:
                mySocket.close()

get_route(hostname)
