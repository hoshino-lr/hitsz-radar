pass_word="rewrite149"
home_dir="hoshino"
cd /home/${home_dir}/radar/build
echo ${pass_word}|sudo -S killall radar_video
python3 -u ./radar_video.py > video.log 2>&1
