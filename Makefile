all:
	g++ main.cxx tcp_connection.cxx parser_conversion.cxx -pthread -O2 -o main.o
	cat README.txt
