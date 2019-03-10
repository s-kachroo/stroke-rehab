const socket = io();

const $messageForm = document.querySelector('#message-form');
const $messageFormInput = $messageForm.querySelector('input');
const $messageFormButton = $messageForm.querySelector('button');
const $sendLocationButton = document.querySelector('#send-location');
const $messages = document.querySelector('#messages')

const messageTemplate = document.querySelector('#message-template').innerHTML

const {username, room} = Qs.parse(location.search, {ignoreQueryPrefix: true})

socket.on('message', (message)=>{
    console.log(message);
    const html = Mustache.render(messageTemplate, {
        message
    })
    $messages.insertAdjacentHTML('beforeend', html)
})

$messageForm.addEventListener('submit', (e)=>{
    e.preventDefault();

    const message = document.querySelector('input').value;
    socket.emit('sendMessage', message, (error)=>{
        $messageFormInput.value = ''
        $messageFormInput.focus()
    });
})

$sendLocationButton.addEventListener('click', ()=>{
    if(!navigator.geolocation){
        return alert('Geolocation not supported')
    }

    navigator.geolocation.getCurrentPosition((position)=>{
        socket.emit('sendLocation', {
            latitude: position.coords.latitude,
            longitude: position.coords.longitude
        }, ()=>{
            console.log('Location Shared!')
        })
    })
})

socket.emit('join', { username, room})