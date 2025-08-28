# Altitude Data Server-Client Assignment Report

## 1. Overview

This assignment implements a **server-client system** to simulate, transmit, and validate altitude data over a network using **TCP sockets** in C. The solution demonstrates:

* Generating altitude data with random turbulence.
* Sending data from server to client asynchronously.
* Receiving and validating data for anomalies in the client.
* Exporting received data to a CSV file.
* Use of **threads** (`pthread`) for asynchronous execution.
* Synchronization with **mutexes** to prevent race conditions.

---

## 2. Server Code (`server.c`)

### Purpose

The server generates altitude data and sends it to the connected client over a TCP socket.

### Key Components

1. **Global Variables**

```c
int altitude_data[TOTAL_POINTS];  // Stores generated altitudes
int data_ready = 0;               // Flag to signal when new data is ready
pthread_mutex_t lock;             // Mutex for thread-safe access
```

2. **Thread: `generate_data`**

* Generates 60 altitude data points (1 per second).
* Simulates smooth increase and random turbulence (`±10 ft`).
* Uses `pthread_mutex_lock/unlock` to update shared data safely.
* Sets `data_ready = 1` to notify the sender thread.

3. **Thread: `send_data`**

* Waits until `data_ready` is set.
* Sends the altitude data to the client over TCP.
* Resets `data_ready = 0` after sending.

4. **Socket Setup**

* Creates a TCP socket (`socket(AF_INET, SOCK_STREAM, 0)`).
* Binds to port 8080 (`bind()`).
* Listens for incoming connections (`listen()`).
* Accepts a single client connection (`accept()`).

5. **Thread Creation**

```c
pthread_create(&gen_thread, NULL, generate_data, NULL);
pthread_create(&send_thread, NULL, send_data, (void *)&new_socket);
```

* Both threads run **asynchronously**.

6. **Cleanup**

* Joins threads (`pthread_join`) to wait for completion.
* Closes the socket and destroys the mutex.

---

## 3. Client Code (`client.c`)

### Purpose

The client receives altitude data from the server, validates it for anomalies, and exports the results to a CSV file.

### Key Components

1. **Global Variables**

```c
int received_data[TOTAL_POINTS];  // Stores received altitudes
int count = 0;                    // Number of points received
int anomalies = 0;                // Count of anomalies detected
pthread_mutex_t lock;             // Mutex for thread safety
```

2. **Thread: `receive_data`**

* Reads incoming TCP data from the server.
* Parses the data line by line (`strtok(buffer, "\n")`).
* Stores the values in `received_data`.
* Uses mutex to ensure safe updates of `count`.

3. **Thread: `validate_data`**

* Monitors `received_data` in real-time.
* Detects anomalies: sudden jumps in altitude > 100 ft.
* Prints warnings when anomalies are detected.
* Uses mutex to safely read shared data.

4. **Socket Setup**

* Creates TCP socket and connects to server at `127.0.0.1:8080`.
* Ensures thread-safe communication with mutexes.

5. **CSV Export**

```c
FILE *f = fopen("altitude_data.csv", "w");
for (int i = 0; i < count; i++) {
    fprintf(f, "%d,%d\n", i + 1, received_data[i]);
}
```

* Exports altitude values with index for reference.

6. **Summary Report**

* Prints total data points received.
* Prints the number of anomalies detected.

---

## 4. Thread Synchronization

* **Mutex (`pthread_mutex_t`)** ensures that shared data is safely accessed by multiple threads.
* Prevents **race conditions** where two threads could read/write the same memory simultaneously.
* `data_ready` flag in server signals the sender thread when new data is ready.

---

## 5. Asynchronous Execution

1. **Server**

   * `generate_data` thread generates data independently.
   * `send_data` thread sends data as it becomes ready.

2. **Client**

   * `receive_data` thread continuously reads data from the server.
   * `validate_data` thread concurrently checks for anomalies in real-time.

* Both server and client threads run concurrently, allowing real-time data processing.

---

## 6. Anomaly Detection Logic

* An anomaly is flagged if:

```c
abs(current_altitude - previous_altitude) > 100
```

* Prints a warning message whenever a sudden jump in altitude occurs.
* Simulates **real-world monitoring of altitude sensors**.

---

## 7. CSV Export

* CSV format:

```
Index,Altitude
1,1002
2,1008
3,1015
...
```

* Enables easy analysis in Excel or plotting tools.

---

## 8. Execution Flow

1. Run `run.sh`:

   * Compiles server and client.
   * Starts server asynchronously.
   * Runs client in foreground.
   * Stops server after client finishes.

2. Server generates and sends altitude data continuously.

3. Client receives, validates, and exports data to CSV.

4. Summary displayed:

   * Total data points received.
   * Number of anomalies detected.

---

## 9. Key Features

* Real-time asynchronous data generation and transmission.
* Multi-threaded design for **efficient parallel processing**.
* Synchronization using mutexes to ensure data integrity.
* Anomaly detection simulating realistic scenarios.
* CSV export for post-processing or plotting.

---

## 10. Summary

This assignment demonstrates:

* **TCP socket programming** in C.
* **Multi-threading** with `pthread`.
* **Data synchronization** using mutexes.
* **Real-time monitoring and validation**.
* **Exporting structured data** for analysis.

The program successfully simulates a real-world system where altitude data is generated, transmitted, validated, and stored for further use.
