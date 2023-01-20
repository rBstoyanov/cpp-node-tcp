const net = require('net'); 
const host = "127.0.0.1";
const port = 10000;
const server = net.createServer(); 

server.listen(port, host, () => { 
    console.log(`listening on ${host}:${port}`); 
}); 


let sockets = []; 
server.on('connection', (socket) => { 
    var clientAddress = `${socket.remoteAddress}:${socket.remotePort}`; 
    console.log(`new client connected: ${clientAddress}`); 
    
    socket.on('data', (data) => {
        console.log(`${clientAddress} for string: ${data}`); 
        try {
          const parsed = JSON.parse(data);
          console.log("got json", parsed);
          socket.write("all ok\n");
        } catch (ex) {
          console.log("error parsing json", ex.message);
          socket.write("error\n");
        }
    });

    socket.on('close', (data) => { 
      console.log(`client disconnectd: ${clientAddress}`); 
    });

    socket.on('error', (err) => { 
        console.log(`Error occurred in ${clientAddress}: ${err.message}`); 
    });
});
