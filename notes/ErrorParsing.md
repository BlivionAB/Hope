# Error Parsing

When should we continue parsing an error?
* We should parse on primary statement, and on failure skip to the semicolon end. And
  expect another primary statement.
  * This means if a statement is missing a semicolon it will still continue to the next 
    statement until it finds a semicolon.