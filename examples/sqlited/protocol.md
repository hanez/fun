Fun SQL TCP Demo Protocol (TSV, line-based)

- Client sends exactly one line with the SQL text terminated by a newline ("\n"). The server reads up to 64 KiB.

Responses

1) Query returning rows (e.g., SELECT):
   RESULT
   col1\tcol2\t...\n
   v11\tv12\t...\n
   ...
   END

   Notes:
   - First line is the literal word RESULT followed by a newline.
   - Second line is a header with column names separated by a single tab ("\t").
   - Each subsequent line is one row; fields are tab-separated. Nil/NULL are encoded as empty strings.
   - The block terminates with a line containing the literal END.

2) Exec/DDL (e.g., INSERT/UPDATE/CREATE):
   OK rc

   Notes:
   - rc is the sqlite3 result code (0 indicates success).

3) Error:
   ERROR message

   Notes:
   - The error message is human-readable and not machine-stable.

General
- Newlines are Unix style ("\n").
- Tabs and newlines in data are replaced with spaces for TSV safety.
- The server closes the connection after sending the response.
