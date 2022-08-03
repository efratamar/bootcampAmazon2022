# bootcampAmazon2022
gaz camera project

Gas leak detection system:
The system receives a record [description]  of the temperature in the air and checks where and if there are gas leaks.
The application uses a dynamic library that manages the reception of the images and processes them  into a video that represents the state of temperatures in colors that symbolize the heat levels.
 The start of recording is effectuated with the help of the PIPELELINE method which contains 5 stages. Each stage is activated by a separate thread. The communication between the different threads is carried out by a column synchronized by means of a binary semaphore.
Each cycle in PIPELINE forms the image of the gas in the room
So every second 250 images are created that are part of a video
The stages of the PIPELINE:
Capture - creating a gas state image in space which is transferred to the RGB stage
RGB_convertor: : Converts the image to RGB format according to the heat degree of the gas and transfers the converted image to the YUV stage.
YUV_convertor: converts an image from RGB format to YUV format and passes it to the DECODER stage

End of recording:
Terminates the recording operation in a controlled manner. After the end of the current cycle, the THREADS will be released and the data structures that were assigned dynamically
Creating an image:
A picture is taken while recording by saving the current picture from the recording, if there is no recording in the background we will start recording - one series in PIPELINE
