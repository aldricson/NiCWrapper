#include <cstring>

// Define a function that converts a pair of uint16_t values to a float.
// The function is declared as 'static inline' for optimization.
static inline float unsignedIntPairToFloat(const uint16_t *src)
{
    // Declare a float variable 'f' to store the final floating-point value.
    float f;
    
    // Declare a uint32_t variable 'i' to temporarily store the combined bytes.
    uint32_t i;
    
    // Declare four uint8_t variables to store individual bytes.
    uint8_t a, b, c, d;

    // Extract the high byte from the first uint16_t and store it in 'a'.
    // The operation '>> 8' shifts the bits 8 positions to the right, effectively isolating the high byte.
    // The operation '& 0xFF' masks all but the lowest 8 bits, ensuring 'a' contains only the high byte.
    a = (src[0] >> 8) & 0xFF;
    
    // Extract the low byte from the first uint16_t and store it in 'b'.
    // The operation '& 0xFF' masks all but the lowest 8 bits.
    b = (src[0] >> 0) & 0xFF;
    
    // Extract the high byte from the second uint16_t and store it in 'c'.
    c = (src[1] >> 8) & 0xFF;
    
    // Extract the low byte from the second uint16_t and store it in 'd'.
    d = (src[1] >> 0) & 0xFF;

    // Combine the four bytes into a single uint32_t in BADC order.
    // Each byte is shifted to its final position and then combined using bitwise OR.
    // 'b' is shifted 24 bits to the left to occupy the highest byte in 'i'.
    // 'a' is shifted 16 bits, 'd' is shifted 8 bits, and 'c' remains in its position.
    i = (b << 24) | (a << 16) | (d << 8) | (c << 0);
    
    // Use memcpy to copy the 4 bytes from 'i' into 'f'.
    // This effectively reinterprets the bit pattern of 'i' as a float.
    std::memcpy(&f, &i, 4);

    // Return the final float value.
    return f;
}
