.PHONY: compile-client

compile-client: hashClient.c 
	gcc -Wall -O2 hashClient.c -o hashClient

run-server-noport: hashServer.py 
	python3 hashServer.py

run-server-port: hashServer.py
	python3 hashServer.py -p 3333
	
clean: 
	rm hashClient