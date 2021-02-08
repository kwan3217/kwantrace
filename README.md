# KwanTrace
A C++ Ray-tracer

It occurred to me that most of my worries over POV-Ray are due to:

 * Parsing time
 * Memory usage
 * Sharing between frames

All of these could be eliminated by changing the parser up. What if instead
of a custom scene description language, we just used C++? The program would
construct the scene, then call the render routine to do the actual rendering.

    Shader s();
    Cone c(vector(),radius1,radius2);
    s.add(c);
    // Lots more stuff
    s.render();

*Then, it could modify the existing scene and render it again.*

    c.radius1=clock*0.2;
    s.render();

