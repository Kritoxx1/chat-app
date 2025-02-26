# Chat App

A simple chat application built using C and sockets for communication.
This project is a work in progress, and I plan to improve it over time.

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/Kritoxx1/chat-app.git
   cd chat-app
   ```
2. Compile the server and client:
   ```sh
   gcc server.c -o server -pthread
   gcc client.c -o client -pthread
   ```

## Usage
### Running the Server
```sh
./server
```

### Running the Client
```sh
./client <USERNAME> <ID>
```
Example:
```sh
./client Alice 1234
```

## Contributing
Feel free to fork this repository and submit pull requests with improvements!

## License
This project is licensed under the MIT License.

