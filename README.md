# Advanced C Project: Contact Management Application

## Description

This project involves creating a client-server application in C for managing contacts. The client connects to the server, authenticates with a login and password, and performs actions based on the user's role (Admin or Guest). The application allows admins to manage contacts and guests to view and search contacts.

## Features

### Client-Side Application
- **User Authentication**:
  - Prompts for login and password.
  - Allows up to 3 attempts for correct password.
  - Retrieves user profile (Admin or Guest) upon successful login.
- **Admin Functions**:
  - Displays an administrative menu.
  - Allows the admin to search contacts, view all contacts, add a contact, delete a contact, and modify a contact.
- **Guest Functions**:
  - Allows guests to view and search contacts.

### Server-Side Application
- **Connection Handling**:
  - Listens for client connections.
- **Authentication Verification**:
  - Verifies login credentials.
  - Provides a personalized menu based on user profile.
- **Contact Management**:
  - Adds, modifies, deletes, searches, and displays all contacts.

## User Accounts for Authentication

- **Admin Account**:
  - Login: `admin1`
  - Password: `0011`
- **Guest Account**:
  - Login: `invite1`
  - Password: `0011`

## Running the Application

1. **Run the server code**:


2. **Run the client code**:


## Authors

This project is done by:
- Ilias Maroun
- Rajae El-Abida
- Mohamed El Hauari
- Mohamed Sadki

## License

This project is licensed under the MIT License.
