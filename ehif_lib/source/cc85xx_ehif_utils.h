/** \addtogroup module_ehif_utils Utilities
 *
 * \brief Non-EHIF/MCU specific utility macros and definitions
 *
 * @{
 */
#ifndef CC85XX_EHIF_UTILS_H_
#define CC85XX_EHIF_UTILS_H_


//-------------------------------------------------------------------------------------------------------
/// \name Useful definitions
//@{

#ifndef NULL
    /// Null-pointer
    #define NULL            ((void*) 0)
#endif
//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name Utility Macros
//@{

/// Used for macro encapsulation
#define st(x)               do { x } while (0)

/// Returns minimum of two values
#define MIN(n,m)            (((n) < (m)) ? (n) : (m))
/// Returns maximum of two values
#define MAX(n,m)            (((n) < (m)) ? (m) : (n))
/// Returns the absolute value
#define ABS(n)              (((n) < 0) ? -(n) : (n))

/// Returns a bit-vector with bit "n" set and all other bits cleared
#define BV(n)               ((uint32_t) 1 << (n))

/// Returns the most significant uint16_t in a uint32_t
#define HI16(x32)           ((uint16_t) (((uint32_t) (x32)) >> 16))
/// Returns the least significant uint16_t in a uint32_t
#define LO16(x32)           ((uint16_t) ((uint32_t) (x32)))
/// Returns the most significant uint8_t in a uint16_t
#define HI8(x16)            ((uint8_t) (((uint16_t) (x16)) >> 8))
/// Returns the least significant uint8_t in a uint16_t
#define LO8(x16)            ((uint8_t) ((uint16_t) (x16)))

/// Swaps uint16_t between big-endian and little-endian
#define END_SWAP_16(x)       ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0x00FF))
/// Swaps uint32_t between big-endian and little-endian
#define END_SWAP_32(x)       ((((x) << 24) & 0xFF000000) | (((x) <<  8) & 0x00FF0000) | (((x) >> 24) & 0x000000FF) | (((x) >>  8) & 0x0000FF00))

//@}
//-------------------------------------------------------------------------------------------------------


#endif
//@}
