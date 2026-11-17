# Workspace Manual Tests

These tests require the workspace pwd implementation.

## Test 1: Initial directory

Input: pwd

Expected: The output is /.

## Test 2: Repeated pwd

Input pwd twice.

Expected: Both commands print /.

## Test 3: Unknown command does not change the directory

Input these commands on separate lines:
pwd
hello
pwd
exit

Expected:
- Both pwd commands print /.
- hello prints an error.
- The program continues until exit.
