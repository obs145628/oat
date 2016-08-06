var net = require("net");
var dgram = require("dgram");




net.createServer(function(socket) {

    console.log("Client connedted: " + socket.remoteAddress + ":" + socket.remotePort);

    socket.on('data', function(data) {
        var arr = /^EMULATE_LONG_COMP_OP\s(\d+)\n$/m.exec(data);

        var closed = false;

        if(arr)
        {
            var duration = parseInt(arr[1]);
            console.log("Wait received: " + duration);
            setTimeout(function() {
                if(closed)
                    return;
                console.log("Timeout reached: " + duration);
                socket.write("<Ok>\n");
            }, duration * 1000);
        }
        else
        {
            console.log("Invalid format");
            socket.write("<Err>\n");
        }
    });

    socket.on('end', function(data) {
        closed = true;
        console.log('Client disconnected !');
    });

}).listen(3333);

console.log("Server listening on port 3333");



/*

var udpSend = function(sock, message, address, port)
{
    sock.send(message, 0, message.length, port, address, function(err) {
        if(err)
            throw err;
    });
}

var server = dgram.createSocket('udp4');

server.on('listening', function() {
    console.log("Server listening on port 3333");
});

server.on("message", function(data, remote) {

    console.log("Message from " + remote.address + ":" + remote.port);

    var arr = /^EMULATE_LONG_COMP_OP\s(\d+)\n$/m.exec(data);

    if(arr)
    {
        var duration = parseInt(arr[1]);
        console.log("Wait received: " + duration);
        setTimeout(function() {
            console.log("Timeout reached: " + duration);
            udpSend(server, "<Ok>\n", remote.address, remote.port);
        }, duration * 1000);
    }
    else
    {
        console.log("Invalid format");
        updSend(server, "<Err>\n", remote.address, remote.port);
    }
});

server.bind(3333, "127.0.0.1");

*/
