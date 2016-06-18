rm -f mc_read_1_1.txt
rm -f mc_read_1_1.txt.diff
rm -f mc_read_1_2.txt
rm -f mc_read_1_2.txt.diff
rm -f mc_read_2_1.txt
rm -f mc_read_2_1.txt.diff
rm -f mc_read_2_2.txt
rm -f mc_read_2_2.txt.diff
rm -f zmq_read_1_1.txt
rm -f zmq_read_1_1.txt.diff
rm -f zmq_read_1_2.txt
rm -f zmq_read_1_2.txt.diff
rm -f zmq_read_2_1.txt
rm -f zmq_read_2_1.txt.diff
rm -f zmq_read_2_2.txt
rm -f zmq_read_2_2.txt.diff

killall zmq2stdout zmq2file mc2stdout mc2file stdin2zmq stdin2mc file2zmq file2mc 2>/dev/null

