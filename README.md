### Grupa: 323 CA
### Nume: Constantinescu Delia-Georgiana
### Tema 4 PCom

For this homework I used the code from *lab9 - HTTP* as a starting
point, modifying it in order to fit the required functionality of
the application.

For the JSON object parsing, I used the `parson.c` library, because
of compatibility reasons, as this application is written in C. The
library is available here: [https://github.com/kgabis/parson]

## Client:
- I initialized some variables that will be used in the HTTP requests
sent to the server, such as: 
    - `body_data`: used for POST requests
    - `cookies`: used to memorize the login session cookie
    - `token`: used to memorize the library access token
    - `username` and `password`: client credentials
    - `value` and `object`: used for parsing strings to JSON format

- Possible commands from stdin:
    - *exit*: exits the application and frees all additional resources
    - *register*: offers prompts for user to input credetials; computes
    POST request containing credetials; receives response from server;
    prints a corresponding message depending on the server response
    - *login*: offers prompts for user to input credetials; computes
    POST request containing credetials; receives response from server;
    stores session cookie or, in case of failure, prints an error
    message
    - *logout*: checks if user is logged in; computes GET request
    containing session cookie; receives response from server; resets
    any auxiliary variables
    - *enter_library*: checks if user is logged in; computes GET
    request containing session cookie; receives response from server;
    stores access token or, in case of failure, prints an error message
    - *get_books*: checks if user is logged in and has a library access
    token; computes GET request containing session cookie and token;
    prints a list of all available books in the user's library
    - *get_book*: checks if user is logged in and has a library access
    token; offers prompt for user to input the desired book id; assembles
    URL to the respective book; computes GET request containing session
    cookie and token; prints information about the book or, in case of
    failure, an error message
    - *add_book*: checks if user is logged in and has a library access
    token; offers prompt for user to input the details about the book;
    computes POST request containing session cookie, token and book
    object; receives response from server; prints success/error message
    - *delete_book*: checks if user is logged in and has a library access
    token; offers prompt for user to input the desired book id; assembles
    URL to the respective book; computes DELETE request containing session
    cookie and token; prints success/error message
