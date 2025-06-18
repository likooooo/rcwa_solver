#!/usr/bin/python3

# When compiled with -DENABLE_RS_TRACE, the trace output can be formatted
# with this script into an indented form.
# ./example  2>&1 | perl ../pretty_trace.py

level = 0

while True:
    try:
        line = input()
        if line.startswith('>'):
            print(' ' * level + line)
            level += 1
        elif line.startswith('<'):
            level -= 1
            print(' ' * level + line)
        else:
            print(' ' * level + line)
    except EOFError:
        break