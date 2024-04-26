#include <glm/glm.hpp>

// Once again i thank (this stackoverflow answer)[https://stackoverflow.com/a/6930407]
glm::vec3 toHsv(glm::vec3 in) {
	glm::vec3 out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.z = max;                                // v
    delta = max - min;
    if (delta < 0.00001) {
        out.y = 0;
        out.x = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.y = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined (we use 0)
        out.y = 0.0;
        out.x = 0.0;
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.x = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.x = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.x = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.x /= 6.0;                              // 0..1

    if( out.x < 0.0 )
        out.x += 1.0;

    return out;
}

glm::vec3 toRgb(glm::vec3 in) {
    double      hh, p, q, t, ff;
    long        i;
    glm::vec3         out;

    if(in.y <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.z;
        out.g = in.z;
        out.b = in.z;
        return out;
    }
    in.x = std::max(std::min(in.x, 1.f), 0.f);
    in.y = std::max(std::min(in.y, 1.f), 0.f);
    in.z = std::max(std::min(in.z, 1.f), 0.f);
    hh = in.x;
    if(hh >= 1.0) hh = 0.0;
    hh *= 6.0;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.z;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.z;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.z;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.z;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.z;
        break;
    case 5:
    default:
        out.r = in.z;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}