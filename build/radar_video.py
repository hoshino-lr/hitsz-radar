import cv2
#引入库
cap1 = cv2.VideoCapture(2)
cap2 = cv2.VideoCapture(0)
win1 = "Video1"
win2 = "Video2"
cv2.namedWindow(win1)
cv2.namedWindow(win2)
cv2.moveWindow(win1,1280,0)
cv2.moveWindow(win2,0,580)
while True:
    ret1, frame1 = cap1.read()
    ret2, frame2 = cap2.read()
    if ret1:
        cv2.imshow(win1, frame1)
    if ret2:
        cv2.imshow(win2, frame2)
#读取内容
    if cv2.waitKey(10) == ord("q"):
        break
        
#随时准备按q退出
cap1.release()
cap2.release()
cv2.destroyAllWindows()
#停止调用，关闭窗口
