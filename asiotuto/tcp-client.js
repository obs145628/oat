var net = require("net");

console.log("Connection ...");

function testServer(client, message)
{
    console.log("Request: '"  + message + "'");
    console.log("Sending request to server");
    client.write(message);
    console.log("Waiting for response");
}

var client = net.createConnection({port: 3333}, function() {

    client.on("data", function(data) {
        console.log("Response: '" + data + "'");
    });

    testServer(client,"Hello, World !\n");
    testServer(client,"Hello my World !\n");

});


