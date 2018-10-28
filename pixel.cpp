#include "pixel.h"

pixel::pixel()
{

}

pixel pixel::operator = (pixel p){
   red=p.red;
   green=p.green;
   blue=p.blue;

   return *this;
}
bool pixel::operator == (pixel p){
   return ((red == p.red) && (green == p.green) && (blue == p.blue));
}
bool pixel::operator != (pixel p){
   return !((red == p.red) && (green == p.green) && (blue == p.blue));
}
uint16_t pixel::diff(pixel p){
   uint16_t result = 0;
   if(red > p.red){
       result += red-p.red;
   }
   else{
       result += p.red - red;
   }
   if(green > p.green){
       result += green-p.green;
   }
   else{
       result += p.green - green;
   }
   if(blue > p.blue){
       result += blue-p.blue;
   }
   else{
       result += p.blue - blue;
   }
   return result;
}
