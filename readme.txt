A basic syncronisation server/client

A given folder is scanned recursively and the sha1 sum and the filepath of each file is sent to the client.
The client checks if the file exists, if not the file will be sent from the server to the client.

The current version uses QDatastream in version Qt_4_0 so the clients should use that version too.
The data size is limited to quint32, so the max size of a file can be 512MB.
Example usage:
to play the QMemory game over network each player needs to have the same cards. So run the Sync process when initializing a networkgame.
