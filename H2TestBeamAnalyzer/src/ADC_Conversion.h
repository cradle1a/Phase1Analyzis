/* ADC_Conversion.h, Version 4
 * Nathaniel Chaverin
 * Created: 7/25/15
 * Updated: 7/27/15

 * V2--Summary of changes:
 *   Changed fC to floats
 *   Added bracket operator
 * V3--Summary of changes:
 *   Changed inputCharge array to no longer perform
 *       automatic pedestal subtraction
 *   Added copy constructor, operator= and destructor
 * V4--Summary of changes:
 *   Added getArray function
*/

#ifndef ADC_CONVERSION_H_INCLUDED
#define ADC_CONVERSION_H_INCLUDED

#include <cmath>
#include <iostream>

// Bitmasks for 8-bit ADC input
const char expMask = 192;
const char manMask = 63;

const double baseSensitivity = 3.1;

// Base charges for each subrange
// Use array for which 0 ADC = 0 fC input charge
static float inputCharge[] =
{
    -1.6, 48.4, 172.4, 433.4, 606.4,
    517, 915, 1910, 3990, 5380,
    4780, 7960, 15900, 32600, 43700,
    38900, 64300, 128000, 261000, 350000
};

// Defines the size of the ADC mantissa subranges
static int adcBase[] =
{
    0, 16, 36, 57, 64
};

// A class to convert ADC to fC
class Converter
{
private:
    float fc[256];

public:
    // Constructor
    // Populates the fc array
    Converter(double gain = 1.0)
    {
        // Loop over exponents, 0 - 3
        for(int exp = 0; exp < 4; exp++)
        {
            // Loop over mantissas, 0 - 63
            for(int man = 0; man < 64; man++)
            {
                int subrange = -1;
                double sensitivity;

                // Find which subrange the mantissa is in
                for(int i = 0; i < 4; i++)
                {
                    if(man >= adcBase[i] && man < adcBase[i + 1])
                    {
                        subrange = i;
                    }
                }
                if(subrange == -1)
                {
                    std::cout << "Something has gone horribly wrong!" << std::endl;
                    std::cin.get();
                }

                // Sensitivity = 3.1 * 8^exp * 2^subrange
                sensitivity = baseSensitivity * pow(8.0, double(exp)) * pow(2.0, subrange);

                // Add sensitivity * (location in subrange) to base charge
                fc[exp * 64 + man] = (inputCharge[exp * 5 + subrange] + ((man - adcBase[subrange])) * sensitivity) / gain;
                //fc[exp * 64 + man] = inputCharge[exp * 5 + subrange] + ((man - adcBase[subrange]) + .5) * sensitivity;
            }
        }
    }

    float linearize(const unsigned char adc) const
    {
        return fc[adc];
    }

};

#endif // ADC_CONVERSION_H_INCLUDED
