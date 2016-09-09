const http = require("http");

const server = http.createServer((req, res) => {

  console.log("=======Request=======");
  console.log("Version: " + req.httpVersion);
  console.log("Method: " + req.method);
  console.log("Path: " + req.url);
  console.log("Headers: " + JSON.stringify(req.headers, null, 2));
  console.log("=====================");

  res.end("Hello, World !");

});

server.listen(8080);
