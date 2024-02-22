#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

#include "address.hh"
#include "socket.hh"

using namespace std;

void get_URL( const string& host, const string& path )
{
  cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  cerr << "Warning: get_URL() has not been implemented yet.\n";
  Address address(host, "http");
  // Address address(host, 80);
  TCPSocket tcp_socket;
  // tcp_socket.bind(address);
  // tcp_socket.listen();
  // auto ts = tcp_socket.accept();
  tcp_socket.connect(address);
  string send_content = "GET " + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n"; 
  // cerr << send_content;
  tcp_socket.write(send_content); 
  string ret = "";
  while (1) {
    string payload;
    payload.resize(1024);
    tcp_socket.read(payload);
    // cerr << "size:" << payload.size() << "\r\n";
    if (payload.size() == 0) break;
    ret += payload;
  }
  std::cout << ret ;
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
