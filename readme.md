# IoDeMux

NOTE: This is very quickly put together for testing purposes. I've never worked with ncurses before, and have little experience with linux process syscalls. However this project is created to 're-sharpen' these skills. This may receive further updates in the future, but may not as well.

## General

This piece of software will start a subprocess with your provided arguments and provide a TUI that will allow you to input text and send that to stdin of your process. This all while at the same time read out stdout and stderr from the subprocess and display it as well.


The TUI is made with ncurses and the layout looks like:

```text
<Your stdout and stderr output>
...
...



=====================================

>>> <Your stdin input here>
```

## Build / Run

It should all run fairly easily by just running the command `make run` This will build and run the project in one go.

This command will also run with the test python application for the subprocess that is being run. This process will return the input and show a message very 2 seconds.


## Motivation
I was looking for a way to split the input from the output for certain processes. After some googling there was no clear method of doing this.This seemed to be an good solution.
