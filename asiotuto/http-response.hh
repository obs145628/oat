#ifndef HTTP_RESPONSE_HH_
# define HTTP_RESPONSE_HH_

# include <map>
# include <string>
# include <iostream>
# include <boost/asio.hpp>
# include "http-callback.hh"

class HTTPResponse
{
   friend class HTTPRequest;

   HTTPResponse();

public:

   unsigned int getStatusCode() const;
   const std::string& getStatusMessage() const;
   const std::map<std::string, std::string>& getHeaders() const;
   const std::istream& getResponse() const;

private:
   boost::asio::streambuf& getResponseBuf();

   void setStatusCode(unsigned int statusCode);
   void setStatusMessage(const std::string& statusMessage);
   void addHeader(const std::string& name, const std::string& value);

private:
   unsigned int _statusCode;
   std::string _statusMessage;
   std::map<std::string, std::string> _headers;
   boost::asio::streambuf _resBuf;
   std::istream _resStream;

};

#endif //HTTP_RESPONSE_HH_
