# Apache Symlink Race Condition Vulnerability Patch

There is a race condition vulnerability in Apache httpd 2.2.26 (also present in
previous releases) that allows a malicious user to serve arbitrary files from
nearly anywhere on a server that isn't protected by strict os level permissions.
The patch in this repo fixes that vulnerability.

# License

Apache v2.0
