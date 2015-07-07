 
#include "mlx90614.h"




MLX90614::MLX90614(I2C* i2c,int addr){

    this->i2caddress = addr;
    this->i2c = i2c; 
    
}


bool MLX90614::getTemp(float* temp_val){

    char p1,p2,p3;
    float temp_thermo;
    bool ch;

    i2c->stop();                            //stop i2c if not ack
    wait(0.01);
    i2c->start();                           //start I2C                   
    ch=i2c->write(i2caddress);              //device address with write condition
    
    if(!ch)return false;                    //No Ack, return False
    
    ch=i2c->write(0x07);                    //device ram address where Tobj value is present

    if(!ch)return false;                    //No Ack, return False


    i2c->start();                           //repeat start
    ch=i2c->write(i2caddress|0x01);         //device address with read condition 
    if(!ch)return false;                    //No Ack, return False

    p1=i2c->read(1);     //Tobj low byte
    p2=i2c->read(1);     //Tobj heigh byte
    p3=i2c->read(0);     //PEC
    
    i2c->stop();                            //stop condition
     
    
    temp_thermo=((((p2&0x007f)<<8)+p1)*0.02)-0.01;      //degree centigrate conversion
    *temp_val=temp_thermo-273;                          //Convert kelvin to degree Celsius
    
    return true;                            //load data successfully, return true 
}