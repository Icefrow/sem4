#pragma once

/* ---------------------------------------------------------- */
template<typename Device>
float calcAll(float input, Device & device)
{
    return device.calc(input);
}

/* ---------------------------------------------------------- */
// helps to prepare all elements of devices
template<typename Device, typename ...Devices>
float calcAll(float input, Device & first, Devices & ...rest)
{
    return calcAll(first.calc(input), rest...);
}

/* ---------------------------------------------------------- */
// join two values by their weight
inline
float join(float a, float b, float weightA = 0.5f)
{
    return b + weightA * (a - b);
}

/* ---------------------------------------------------------- */
// normalizedValue=0..1
inline float transformValue(float normalizedValue, float rangeMin, float rangeMax)
{
    return join(rangeMax, rangeMin, normalizedValue);
}

/* ---------------------------------------------------------- */
// split proccess bitween two devices then join result by some weight
template<typename DeviceA, typename DeviceB>
float split(float input, DeviceA & deviceA, DeviceB & deviceB, float weightA = 0.5f)
{
    return join(deviceA.calc(input),  deviceB.calc(input), weightA);
}