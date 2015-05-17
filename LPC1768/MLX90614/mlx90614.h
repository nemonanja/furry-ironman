#include "mbed.h"

//Melexis Infrared Thermometer MLX90614 Library

//*****************************************************************
//  Build : 2011-06-08 Hikaru Sugiura
//          Only read thermo data.
//  
//  This program is based on Mr.Mitesh Patel's "mlx90614".
//  http://mbed.org/users/mitesh2patel/programs/mlx90614/lqnetj
//
//  This program does not check CRC.
//  If you want to check CRC, please do it your self :)
//****************************************************************//

/**An Interface for MLX90614
* 
* @code
* //Print temperature data
* #include "mbed.h"
* #include "mlx90614.h"
*
* I2C i2c(p28,p27);   //sda,scl
* MLX90614 thermometer(&i2c);
* float temp;
*
* void main(void){
*   if(thermometer.getTemp(&temp)){
*       printf("Temperature : %f \r\n",temp);
*   }
*   wait(0.5);
*
* }
* @endcode
*/


class MLX90614{

    public:
        /** Create MLX90614 interface, initialize with selected I2C port and address.
        *
        * @param i2c I2C device pointer
        * @param addr Device address(default=0xB4)  
        */    
        MLX90614(I2C* i2c,int addr=0xB4);
        
        /** Get Temperature data from MLX90614. 
        *
        * @param temp_val return valiable pointer
        * @return 0 on success (ack), or non-0 on failure (nack)
        */
        bool getTemp(float* temp_val);
        
    private:
       I2C* i2c;
       int i2caddress;

};