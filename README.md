# Simple In-memory-Database
In-memory database similar to Redis

Data Controll:

| Commands      | Discription      |
| ------------- |:-------------|
| SET <name> <value> | Set the variable <name> to the <value> |
| GET <name>     | Print out the value of variable <name> |
| UNSET <name>  | Unset the variable <name> |
| NUMEQUALTO <value> | Print out the number of variables that are currently to <value> |
| END  | exit the program |


Transaction Controll:

| Commands      | Discription      |
| ------------- |:-------------|
| BEGIN      |  Open a new transaction block. Transaction blocks can be nested; |
| ROLLBACK     |  Undo all of the commands issued in the most recent transaction block, and close the block.   |
| COMMIT  | Close all open transaction blocks, permanently applying the changes made in them.       |
