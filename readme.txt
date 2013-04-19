A basic syncronisation server/client

A given folder is scanned recursively and the sha1 sum and the filepath of each file is sent to the client.
The client checks if the file exists, if not the file will be sent from the server to the client.

Example usage:
to play the QMemory game over network each player needs to have the same cards. So run the Sync process when initializing a networkgame.
