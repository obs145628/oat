#ifndef HTTP_HH_
# define HTTP_HH_

# include <boost/asio.hpp>

namespace http_errors
{
   enum http_error_codes
   {
      invalid_response = 1
   };

   class http_errors_category : public boost::system::error_category
   {
   public:
      const char* name() const BOOST_SYSTEM_NOEXCEPT
      {
         return "http_errors";
      }

      std::string message(int e) const
      {
         switch(e)
         {
            case invalid_response:
               return "Server response cannt be parsed.";
               break;
            default:
               return "Unknown error.";
               break;
         }
      }

   };

   const boost::system::error_category& get_http_errors_category();

   boost::system::error_code make_error_code(http_error_codes e);
}

namespace boost
{
   namespace system
   {
      template<>
      struct is_error_code_enum<http_errors::http_error_codes>
      {
         BOOST_STATIC_CONSTANT(bool, value = true);
      };
   }
}

#endif //!HTTP_HH_
