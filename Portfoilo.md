## Resumé - 2016
* [Link to Resumé](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/resume%20-%202016/Aidan%20Melen.pdf)

## Computer Graphics - Ray Tracer
* [Code On GitHub](https://github.com/aidanmelen/computer_graphics)

The images below, which were created with a python ray tracer, demonstrates some key concepts in Computer Graphics. More specifically, this work contains the following attributes: dynamic light source, dynamic origin, intersection with sphere, intersection with a polygon, phong shading, soft shadows, specular reflection, and fuzzy mirrors.

![image 1](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/computer-graphics/project_one.png)

The image above was submitted as my final ray-traced image and contains all of the attributes as the first image, plus texture mapping to polygons. For instance, you can see the the floor of this room is mapped to an image of wood flooring and the polygon on the left wall is mapped to an image of a open window. 

In retrospect, I learned that python wasn’t the best language choice for building a ray tracer because python does not perform well under computationally heavy programs. During the initial test of my ray tracer, It would take anywhere between 6-8 hours to generate and image. This duration was far too long for development. Throughout the semester, I began searching for optimizations so that I could speed up my code. Reluctantly, I stumbled upon pypy, a just in time interpreter or jit for python 2.7. By running my ray tracer with pypy, I recorded my ray tracer generating images 5 times faster than the conventionally python 2.7 interpreter. 


## Embedded Systems - PIR Motion Sensor Box
* [Code On GitHub](https://github.com/aidanmelen/arduino/blob/master/motion_box.ino)
* [Link to Inscrutable Page](http://www.instructables.com/id/PIR-Motion-Sensor-Box/)
* [Link To Video Demonstration](https://vimeo.com/131317907)

<img src="https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/embedded-systems/IMG_2316.jpg" width="400"> <img src="https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/embedded-systems/Untitled%20Sketch_bb.jpg" width="450">
<!--![image 3](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/embedded-systems/IMG_2316.jpg)-->

<!--![image 4](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/embedded-systems/Untitled%20Sketch_bb.jpg)-->

During May term of 2015, I created Proximity Infrared motion sensor box which could used to turn on household appliances when the user enters the room. The embedded system works by reading the input signal from the PIR sensor and when activated, the Arduino Micro activates a relay switch allowing a high current to flow to the external socket. This can run a variety of appliances, such as: a lamp, iHome speaker system, alarm, or whatever your heart desires, so long as the device draws under 30 amps.

This project helped me explore and learn how to program Arduino Microcontrollers from start to finish. More specifically, I learned the process of first prototyping the circuit on a breadboard, then to solder the parts to a perf board where they can remain permanent. Since this project incorporated high voltage currents, I needed to spend much of my time learning proper procedures such that I didn’t injure myself or burn down my apartment. Furthermore, I needed to apply problem solving skills to get the project to incorporate into a project box. All in all, the project was a success and I was able to learn the basic skills needed to start work on my senior project. 


## Software Engineering - Django Web Server
* [Code On GitHub](https://github.com/TheBigTeam/hamath-workspace)

![image 5](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/software-engineering/GamePlay.png)

Software Engineering gave me the opportunity to continue growing my python skill set while learning the Django web-framework. Before this Spring of 2016, I had very little experience working on a web application. During the semester, I was responsible for creating the DOM for all the pages, server-side programming such as hooking urls to views, creating a Postgresql database and integrating it with django's forms and models, a Python Emailer allows teachers to apply for Teacher credentials on signup, css styling, and lastly, setting up and managing the GitHub remote repository.

Since this project was setup in groups of four, it was necessary for me to share and collaborate with my group as much as possible. It was rewarding to share the technologies I explored with my group and the class. 

More specifically, our group had difficulties learning and using GitHub which resulted in many catastrophic merge-conflicts. In fact, our entire database was deleted and entire blocks of code were erased. It was during this time that I realized the true importance of a version control system. I learned that one could simply reset the head of the project back to a state where the project worked thus simply avoiding a catastrophe. I had modest experience with git and mercurial before this course; however, I had never needed to salvage a broken remote repository shared among four programmers. This was a new challenge for me which required problem solving, but also communication with my group members.


##IOS Application Development - Drinking Lab <img src="https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/ios-development/Drinking%20Lab%20-%20iPal.png" width="48">
* [Link to Apple's App Store](https://itunes.apple.com/us/app/drinking-lab-lite/id916412823?mt=8)

![image 7](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/ios-development/Drinking%20Lab%20PRO.png)

With the knowledge gained from a mobile software development course at Westminster College, I was inspired to apply these unique problem solving skills over the summer of 2014. I developed Drinking Lab, a free iPhone application aimed at preventing the misguided and uneducated abuse of alcohol. With a highly advanced formula provided by the National Highway and Traffic Safety Administration, courtesy of Herbert Moskowitz, a respected alcohol researcher, I was able to program an algorithm that calculates blood alcohol content measurements that equate to the results given from police grade breathalyzers. Drinking Lab securely analyzes the user's profile in contrast with the number of alcoholic drinks and actively alerts them when they become dangerously intoxicated, and with the integrated taxi-tracker feature, users can locate and obtain the proper transportation they need. I also added education section complete with scholarly resources, advice, alcohol myths, and drinking statistics with the help of Karnell Black, Westminster’s Director of Student Involvement and Counselor, Cory Shipp.

Currently, Drinking Lab has received a five-star rating on Apple’s App Store, has acquired over 1000 downloads from twenty-four countries worldwide. I have also received many positive reviews; for example, these users shared:

| User          | rating        | comment  |
| ------------- |:-------------:| :--------|
| Rel_horowitz | \*\*\*\*\* | Drinking lab is excellent! I've used many other BAC calculators and this one is by far the most accurate! This app is a must for anyone that drinks! |
| gcubed | \*\*\*\*\*   | Really nice app that informs people and helps drink responsibly. Especially useful for college campuses and students” |
| Ed_hanson | \*\*\*\*\* | This app is very well done and super informative. I like the intuitive layout and plan on using it tonight. Love it |
| j_trees | \*\*\*\*\* | Best app on the market! |

![image 8](https://github.com/aidanmelen/SeniorProject/blob/master/resources/portfolio/ios-development/Screen%20Shot%202016-04-11%20at%209.16.41%20PM.png)

Reading the wonderful feedback from these satisfied users has been one of the most exciting and rewarding experiences in my life thus far. I am proud to say that my iPhone application has the potential to save many lives by steering users away from drunk driving.


## Computer Networks - Http Web Server
* [Code On GitHub](https://github.com/aidanmelen/HttpServer)

During Computer Network of Fall of 2016, we explored socket programming with Java. During this time, I was able to write a xml configurable, multi-threaded web server. Although the the front-end seems rather elementary, the backend is advanced. For instance, the web server is capable of resource caching, it can handle 404 (page not found) and 400 (bad request) errors as well as do server side logging of all the GET request traffic. 

By programming with sockets, we were forced to read streams of HTTP request, parsing for the requested resource, and generating a proper response with the resource that was request. It was a challenging assignment which required many debugging methods. For instance, I could not simply test a 400 error with a browser, instead I needed to use telnet in order to send a faulty GET request. All in all, this assignment really helped me understand the application layer of the network hierarchy and socket programming in Java.


## Artificial Intelligence - Sudoku Solver
* [Code On GitHub](https://github.com/aidanmelen/sudoku_solver)
* [Link To Video Demonstration](https://vimeo.com/123255806)

In 2005, A Mathematician named Bertrand Felgenhauer proved that there are 6,670,903,752,021,072,936,960 or (6.67 * 1021) unique 9x9 sudoku board games or states. Therefore, I realized that creating a brute force sudoku solver is impractical. Instead, I was able to write a Python program which implements backtracking and forward-checking, search optimisations that we covered in Artificial Intelligence, in order to solve sudoku boards in a reasonable amount of time. 

When I was finished writing the code, I experimented with a variety of beginning board state which increased in difficulty. Moreover, I created a little video (as seen in the link above) which demonstrates solving even the most rigorous sudoku games in seconds. Both taking Artificial Intelligence and Algorithms, I was solely comfortable programming in Java; however, during this assignment I was able to diversify my programming experience by learning Python.
