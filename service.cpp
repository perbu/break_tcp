
#include <sstream>

#include <os>
#include <net/interfaces>
#include <timers>

#include <unistd.h> // for sleep.

using namespace std::chrono;


void Service::start()
{
  auto& inet = net::Interfaces::get(0);

  Timers::periodic(5s, 30s,
  [&inet] (uint32_t) {
    printf("<Service> TCP STATUS:\n%s\n", inet.tcp().status().c_str());
  });

  auto& server = inet.tcp().listen(2000);

  server.on_connect(
  [] (net::tcp::Connection_ptr conn) {
    printf("<Service> @on_connect: Connection %s successfully established. Starting slow read.\n",
      conn->remote().to_string().c_str());
    conn->on_read(1024,
    [conn] (auto buf)
    {
      printf("<Service> @on_read: %lu bytes received.\n", buf->size());
      try
      {
        const std::string data((const char*) buf->data(), buf->size());
        printf("<Service> Responding with OK and sleeping.\n");
        conn->write("OK\n");
        usleep(1000000);
      }
      catch(const std::exception& e)
      {
        printf("<Service> Exception:\n%s\n", e.what());
      }
    });
    conn->on_write([](size_t written) {
      printf("<Service> @on_write: %lu bytes written.\n", written);
    });
  });

  printf("*** Breaking demo started. ***\n");
}
