#include "http.hh"

namespace http_errors
{

   const boost::system::error_category& get_http_errors_category()
   {
      static http_errors_category cat;
      return cat;
   }

   boost::system::error_code make_error_code(http_error_codes e)
   {
      return boost::system::error_code(static_cast<int> (e),
                                       get_http_errors_category());
   }

}
