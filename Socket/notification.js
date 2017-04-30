var server     = require('http').createServer(),
    io         = require('socket.io')(server),
    port       = 1333;

// Logger config
console.log('SocketIO > listening on port ' + port);

function handler (req, res) {
    res.writeHead(200);
    res.end("Welcome to socket.io.");
}

io.sockets.on('connection', function (socket){
    console.log('SocketIO > Connected socket ' + socket.id);

    socket.on('disconnect', function () {
        console.log('SocketIO > Disconnected socket ' + socket.id);
    });

    socket.on('broadcast', function (message) {
        io.sockets.emit('pop', { hello: 'world' });
        console.log('ElephantIO broadcast > ' + JSON.stringify(message));
    });
});

server.listen(port);
