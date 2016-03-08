objects1 = ChatServer.o ServerSocket.o Socket.o
objects2 = ChatClients.o ClientSocket.o Socket.o

ChatServer ChatClients : $(objects1) $(objects2)
	g++ -o ChatServer $(objects1) -L/usr/lib64/mysql -lmysqlclient -lpthread
	g++ -o ChatClients $(objects2) -lpthread 
ChatServer.o : ChatServer.cpp
	g++ -c -I /usr/include/mysql ChatServer.cpp
ServerSocket.o : ServerSocket.cpp
	g++ -c -I /usr/include/mysql ServerSocket.cpp

.PHONY : clean
clean :
	-rm $(objects1) $(objects2)

