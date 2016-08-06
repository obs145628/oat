#include "http-response.hh"

HTTPResponse::HTTPResponse()
   : _resStream(&_resBuf)
{

}

unsigned int HTTPResponse::getStatusCode() const
{
   return _statusCode;
}

const std::string& HTTPResponse::getStatusMessage() const
{
   return _statusMessage;
}

const std::map<std::string, std::string>& HTTPResponse::getHeaders() const
{
   return _headers;
}

const std::istream& HTTPResponse::getResponse() const
{
   return _resStream;
}


boost::asio::streambuf& HTTPResponse::getResponseBuf()
{
   return _resBuf;
}

void HTTPResponse::setStatusCode(unsigned int statusCode)
{
   _statusCode = statusCode;
}

void HTTPResponse::setStatusMessage(const std::string& statusMessage)
{
   _statusMessage = statusMessage;
}

void HTTPResponse::addHeader(const std::string& name, const std::string& value)
{
   _headers[name] = value;
}
