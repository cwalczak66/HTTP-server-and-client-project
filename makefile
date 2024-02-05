CC=gcc


client:http_client.c
	$(CC)  client.c -o http_client


http_server:http_server.c
	$(CC)  http_server.c -o http_server

clean:
	rm -f client; rm -f http_server
