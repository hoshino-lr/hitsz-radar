pass_word="rewrite149"
home_dir="hoshino"
cd /home/hoshino/radar/build
echo ${pass_word}|sudo -S killall start_loop
echo ${pass_word}|sudo -S killall main
echo ${pass_word}|sudo -S -E ./start_loop

