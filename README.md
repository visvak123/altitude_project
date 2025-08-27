# Altitude Simulation Project

## Project Overview
This project simulates altitude data every second for 60 seconds and streams it from a server to a client using multithreading in C. The client receives the data and stores it in a CSV file (`altitude_data.csv`).

---

## Compilation Instructions
To compile the server and client programs on Ubuntu:

```bash
gcc server.c -o server -lpthread
gcc client.c -o client -lpthread
