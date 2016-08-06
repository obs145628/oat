#include <boost/asio.hpp>
#include <string>
#include <memory>

void writeToSocket(boost::asio::ip::tcp::socket& sock,
                   const std::string& message);

std::string readFromSocket(boost::asio::ip::tcp::socket& sock, size_t len);

std::string readLineFromSocket(boost::asio::ip::tcp::socket& sock);

void asyncWriteToSocket(std::shared_ptr<boost::asio::ip::tcp::socket> sock,
                        const std::string& message);

void asyncReadFromSocket(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

void writeUniqueToSocket(boost::asio::ip::tcp::socket& sock,
                   const std::string& message);

std::string readUniqueFromSocket(boost::asio::ip::tcp::socket& sock);
