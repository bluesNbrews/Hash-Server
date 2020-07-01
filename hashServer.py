#!/usr/bin/env python3

import argparse
import hashlib
import socket
import sys

HOST = "127.0.0.1"
BUFFER_SIZE = 4096

def checkArgs(args=None):
	'''Process command line arguments for the port number'''
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--port', help='Optional port number', default='2345')
    results = parser.parse_args(args)

    return results.port

def readFile(connection, fileSize, fileName):
	'''Receive the file from the client'''
    totalBytesRead = 0

    f = open(fileName, "wb")
    print(f"Receiving {fileName}...")

    for _ in range(fileSize):
        #Read 4096 bytes at a time 
        bytes_read = connection.recv(BUFFER_SIZE)
        totalBytesRead += len(bytes_read)
        #Write the bytes to a file
        f.write(bytes_read)
        #When the amount of bytes received is the same as the file size, file transmission is done
        if totalBytesRead == fileSize:
            break
    
    f.close()

def hashFile(fileName, hashAlgo):
    '''Hash the specified file with the specified hash algorithm'''
	#Open the file that was written above and read contents
    file = open(fileName, "rb")
    content = file.read()  

    #Hash the file according to the user input
    if hashAlgo == "md5" or hashAlgo == "MD5":
        finalHash = md5(content)
    elif hashAlgo == "sha1" or hashAlgo == "SHA1":
        finalHash = sha1(content)
    elif hashAlgo == "sha256" or hashAlgo == "SHA256":
        finalHash = sha256(content)
    elif hashAlgo == "sha512" or hashAlgo == "SHA512":
        finalHash = sha512(content)

    #Close the file
    file.close()

    #Return the hash value
    return finalHash

def sha1(content):
	'''Hash the file (SHA1)'''
    sha1_hash = hashlib.sha1()
    sha1_hash.update(content)
    digest = sha1_hash.hexdigest()

    return digest

def sha256(content):
    '''Hash the file (SHA256)'''
    sha256_hash = hashlib.sha256()
    sha256_hash.update(content)
    digest = sha256_hash.hexdigest()

    return digest

def sha512(content):
    '''Hash the file (SHA512)'''
    sha512_hash = hashlib.sha512()
    sha512_hash.update(content)
    digest = sha512_hash.hexdigest()

    return digest

def md5(content):
    '''#Hash the file (MD5)'''
    md5_hash = hashlib.md5()
    md5_hash.update(content)
    digest = md5_hash.hexdigest()

    return digest

def main():
	#Get the port number 
    port = int(checkArgs(sys.argv[1:]))
    if port < 0 or port > 65535:
        sys.exit("Please enter a valid port number (0-65535).")

    #Create socket, bind, listen and accept connection from client
    s = socket.socket()
    s.bind((HOST, port))
    print("Listening...")
    s.listen()
    connection, address = s.accept()

    #Display client connection info (IP and port)
    print(f"Connected by {address[0]}:{address[1]}")

    #Receive the filename, hashing algorithm and filesize and clean for use
    hashAlgo = connection.recv(BUFFER_SIZE).decode('UTF-8')
    hashAlgo = hashAlgo.rstrip('\x00')

    #Receive files, hash and send back to the client
    while True: 
        #Receive the the file size and file name
        try:
            fileSize = connection.recv(BUFFER_SIZE).decode('UTF-8')
            fileName = connection.recv(BUFFER_SIZE).decode('UTF-8')
        except:
            break
        #Clean the file size and file name for use
        #If file size and file name are not received, transmission from client is done
        try:
            fileSize = int(fileSize.rstrip('\x00'))
            fileName = fileName.rstrip('\x00')
        except:
            break

        #Read the file
        readFile(connection, fileSize, fileName)

        #Hash the file
        finalHash = hashFile(fileName, hashAlgo)

        #Print to stdout
        print(f"The {hashAlgo} hash of {fileName} is {finalHash}")

        #Send back to the client
        connection.send(finalHash.encode())

    #Close the client and server sockets
    connection.close()
    s.close()

if __name__ == "__main__":
    main()