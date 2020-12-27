from picamera import PiCamera
#https://raspberrypi.stackexchange.com/questions/72308/how-to-stream-video-via-socket-using-opencv-and-picamera
import base64
import cv2
import zmq
context = zmq.Context()
footage_socket = context.socket(zmq.PUB)
footage_socket.connect('tcp://localhost:5555')
camera = PiCamera()
camera.rotation = 180
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera,size=camera.resolution)
for Frame in (camera.capture_continuous(rawCapture,format="bgr",use_video_port=True)):
    try:
        frame = Frame.array
        frame = cv2.resize(frame, (640, 480))  # resize the frame
        encoded, buffer = cv2.imencode('.jpg', frame)
        jpg_as_text = base64.b64encode(buffer)
        footage_socket.send(jpg_as_text)
    except KeyboardInterrupt:
        camera.release()
        cv2.destroyAllWindows()
        break
camera.close()
