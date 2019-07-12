#include <iostream>
#include <boost/asio.hpp>

void print(const boost::system::error_code& /*e*/)
{
  std::cout << "Hello, world!" << std::endl;
}

int main()
{
  boost::asio::io_context io;

  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
  t.async_wait(&print);

  std::cout<< "before run" <<std::endl;
  io.run();
  std::cout<< "after run" <<std::endl;

  return 0;
}
//before run
//Hello, world!
//after run

