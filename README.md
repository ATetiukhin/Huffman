Huffman
=======

Problem statement
-----------------------
The program performing the two-pass compression with Huffman algorithm.

Command-line options
---------------------------
 | | |
 ---------------------|:---------------------------:| 
-c                    | compress                    |
-f                    | uncompress                  |     
-f, --file &lt;path>  | the name of the input file  |     
-o, --output &lt;path>| the name of the output file |

The value of the parameter (if any) specified through the gap.
The program should display the statistics of the compression/decompression:
the original size of the data, the received data size and the size required
to store auxiliary data (e.g., tables) and nothing more.

    $ ./huffman -c -f myfile.txt -o result.bin
    15678
    6172
    482
    $ huffman -u -f result.bin -o myfile_new.txt
    6172
    15678
    482

### Restrictions
The maximum size of the input file is 5MB. The limit on the run time to 5 seconds.