const express = require('express');
const http = require('http');
const path = require('path');
const socketio = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketio(server);

app.use(express.static(__dirname + '/public'));

io.on('connection', (socket) =>{
    console.log('Connected...');
    
    socket.on('join', ({username, room}) => {
        socket.join(room)
        socket.emit('message', 'Welcome');
        socket.broadcast.to(room).emit('message', `${username} added!`);
    })

    socket.on('sendMessage', (message)=>{
        io.emit('message', message);
    })

    socket.on('sendLocation', (coords)=>{
        io.emit('message', `https://www.google.com/maps/?${coords.latitude},${coords.longitude}`);
    })

    socket.on('disconnect',()=>{
        io.emit('message', 'User left');
    })
})
server.listen(3000, ()=>{
    console.log('Port up')
});