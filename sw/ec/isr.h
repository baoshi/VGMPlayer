/* 
 * File:   isr.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:36 PM
 */

#ifndef ISR_H
#define	ISR_H

#ifdef	__cplusplus
extern "C"
{
#endif


#define enable_global_int() do { INTCONbits.GIE = 1; } while (0)
#define disable_global_int() do { INTCONbits.GIE = 0; } while (0)
#define enable_peripheral_int() do { INTCONbits.PEIE = 1;} while (0)
#define disable_peripheral_int() do { INTCONbits.PEIE = 0; } while (0)


#ifdef	__cplusplus
}
#endif

#endif	/* ISR_H */

