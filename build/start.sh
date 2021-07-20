pass_word="rewrite149"
home_dir="hoshino"
cd /home/${home_dir}/radar/build
#echo ${pass_word}|sudo -S nohup ./start_loop "./main" >warm_up.txt 2>&1 &
#sleep 45s
echo ${pass_word}|sudo -S killall start_loop
echo ${pass_word}|sudo -S killall main 
echo ${pass_word}|sudo -S ./start_loop "./main"
