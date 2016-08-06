var net = require("net");

net.createServer(function(socket) {

    var name = socket.remoteAddress + ":" + socket.remotePort;

    console.log("Client connection: " + name);

    socket.on('data', function(data) {
        console.log("Client request: " + name);
        console.log("Content: " + data);
        socket.write(data.toString().toUpperCase());
    });

    socket.on('end', function(data) {
        console.log("Client disctionnection: " + name);
    });

}).listen(3333);

console.log("Server listening on port 3333");
