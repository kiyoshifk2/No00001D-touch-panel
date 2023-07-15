/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************

/*** Macros for GPIO_RB0 pin ***/
#define GPIO_RB0_Set()               (LATBSET = (1<<0))
#define GPIO_RB0_Clear()             (LATBCLR = (1<<0))
#define GPIO_RB0_Toggle()            (LATBINV= (1<<0))
#define GPIO_RB0_Get()               ((PORTB >> 0) & 0x1)
#define GPIO_RB0_OutputEnable()      (TRISBCLR = (1<<0))
#define GPIO_RB0_InputEnable()       (TRISBSET = (1<<0))
#define GPIO_RB0_PIN                  GPIO_PIN_RB0
/*** Macros for GPIO_RB9 pin ***/
#define GPIO_RB9_Set()               (LATBSET = (1<<9))
#define GPIO_RB9_Clear()             (LATBCLR = (1<<9))
#define GPIO_RB9_Toggle()            (LATBINV= (1<<9))
#define GPIO_RB9_Get()               ((PORTB >> 9) & 0x1)
#define GPIO_RB9_OutputEnable()      (TRISBCLR = (1<<9))
#define GPIO_RB9_InputEnable()       (TRISBSET = (1<<9))
#define GPIO_RB9_PIN                  GPIO_PIN_RB9
/*** Macros for GPIO_RB10 pin ***/
#define GPIO_RB10_Set()               (LATBSET = (1<<10))
#define GPIO_RB10_Clear()             (LATBCLR = (1<<10))
#define GPIO_RB10_Toggle()            (LATBINV= (1<<10))
#define GPIO_RB10_Get()               ((PORTB >> 10) & 0x1)
#define GPIO_RB10_OutputEnable()      (TRISBCLR = (1<<10))
#define GPIO_RB10_InputEnable()       (TRISBSET = (1<<10))
#define GPIO_RB10_PIN                  GPIO_PIN_RB10
/*** Macros for GPIO_RB11 pin ***/
#define GPIO_RB11_Set()               (LATBSET = (1<<11))
#define GPIO_RB11_Clear()             (LATBCLR = (1<<11))
#define GPIO_RB11_Toggle()            (LATBINV= (1<<11))
#define GPIO_RB11_Get()               ((PORTB >> 11) & 0x1)
#define GPIO_RB11_OutputEnable()      (TRISBCLR = (1<<11))
#define GPIO_RB11_InputEnable()       (TRISBSET = (1<<11))
#define GPIO_RB11_PIN                  GPIO_PIN_RB11
/*** Macros for GPIO_RB12 pin ***/
#define GPIO_RB12_Set()               (LATBSET = (1<<12))
#define GPIO_RB12_Clear()             (LATBCLR = (1<<12))
#define GPIO_RB12_Toggle()            (LATBINV= (1<<12))
#define GPIO_RB12_Get()               ((PORTB >> 12) & 0x1)
#define GPIO_RB12_OutputEnable()      (TRISBCLR = (1<<12))
#define GPIO_RB12_InputEnable()       (TRISBSET = (1<<12))
#define GPIO_RB12_PIN                  GPIO_PIN_RB12
/*** Macros for GPIO_RF4 pin ***/
#define GPIO_RF4_Set()               (LATFSET = (1<<4))
#define GPIO_RF4_Clear()             (LATFCLR = (1<<4))
#define GPIO_RF4_Toggle()            (LATFINV= (1<<4))
#define GPIO_RF4_Get()               ((PORTF >> 4) & 0x1)
#define GPIO_RF4_OutputEnable()      (TRISFCLR = (1<<4))
#define GPIO_RF4_InputEnable()       (TRISFSET = (1<<4))
#define GPIO_RF4_PIN                  GPIO_PIN_RF4
/*** Macros for GPIO_RF5 pin ***/
#define GPIO_RF5_Set()               (LATFSET = (1<<5))
#define GPIO_RF5_Clear()             (LATFCLR = (1<<5))
#define GPIO_RF5_Toggle()            (LATFINV= (1<<5))
#define GPIO_RF5_Get()               ((PORTF >> 5) & 0x1)
#define GPIO_RF5_OutputEnable()      (TRISFCLR = (1<<5))
#define GPIO_RF5_InputEnable()       (TRISFSET = (1<<5))
#define GPIO_RF5_PIN                  GPIO_PIN_RF5
/*** Macros for GPIO_RF3 pin ***/
#define GPIO_RF3_Set()               (LATFSET = (1<<3))
#define GPIO_RF3_Clear()             (LATFCLR = (1<<3))
#define GPIO_RF3_Toggle()            (LATFINV= (1<<3))
#define GPIO_RF3_Get()               ((PORTF >> 3) & 0x1)
#define GPIO_RF3_OutputEnable()      (TRISFCLR = (1<<3))
#define GPIO_RF3_InputEnable()       (TRISFSET = (1<<3))
#define GPIO_RF3_PIN                  GPIO_PIN_RF3
/*** Macros for GPIO_RF2 pin ***/
#define GPIO_RF2_Set()               (LATFSET = (1<<2))
#define GPIO_RF2_Clear()             (LATFCLR = (1<<2))
#define GPIO_RF2_Toggle()            (LATFINV= (1<<2))
#define GPIO_RF2_Get()               ((PORTF >> 2) & 0x1)
#define GPIO_RF2_OutputEnable()      (TRISFCLR = (1<<2))
#define GPIO_RF2_InputEnable()       (TRISFSET = (1<<2))
#define GPIO_RF2_PIN                  GPIO_PIN_RF2
/*** Macros for GPIO_RF6 pin ***/
#define GPIO_RF6_Set()               (LATFSET = (1<<6))
#define GPIO_RF6_Clear()             (LATFCLR = (1<<6))
#define GPIO_RF6_Toggle()            (LATFINV= (1<<6))
#define GPIO_RF6_Get()               ((PORTF >> 6) & 0x1)
#define GPIO_RF6_OutputEnable()      (TRISFCLR = (1<<6))
#define GPIO_RF6_InputEnable()       (TRISFSET = (1<<6))
#define GPIO_RF6_PIN                  GPIO_PIN_RF6
/*** Macros for GPIO_RD9 pin ***/
#define GPIO_RD9_Set()               (LATDSET = (1<<9))
#define GPIO_RD9_Clear()             (LATDCLR = (1<<9))
#define GPIO_RD9_Toggle()            (LATDINV= (1<<9))
#define GPIO_RD9_Get()               ((PORTD >> 9) & 0x1)
#define GPIO_RD9_OutputEnable()      (TRISDCLR = (1<<9))
#define GPIO_RD9_InputEnable()       (TRISDSET = (1<<9))
#define GPIO_RD9_PIN                  GPIO_PIN_RD9
/*** Macros for GPIO_RD10 pin ***/
#define GPIO_RD10_Set()               (LATDSET = (1<<10))
#define GPIO_RD10_Clear()             (LATDCLR = (1<<10))
#define GPIO_RD10_Toggle()            (LATDINV= (1<<10))
#define GPIO_RD10_Get()               ((PORTD >> 10) & 0x1)
#define GPIO_RD10_OutputEnable()      (TRISDCLR = (1<<10))
#define GPIO_RD10_InputEnable()       (TRISDSET = (1<<10))
#define GPIO_RD10_PIN                  GPIO_PIN_RD10
/*** Macros for GPIO_RD0 pin ***/
#define GPIO_RD0_Set()               (LATDSET = (1<<0))
#define GPIO_RD0_Clear()             (LATDCLR = (1<<0))
#define GPIO_RD0_Toggle()            (LATDINV= (1<<0))
#define GPIO_RD0_Get()               ((PORTD >> 0) & 0x1)
#define GPIO_RD0_OutputEnable()      (TRISDCLR = (1<<0))
#define GPIO_RD0_InputEnable()       (TRISDSET = (1<<0))
#define GPIO_RD0_PIN                  GPIO_PIN_RD0
/*** Macros for GPIO_RD1 pin ***/
#define GPIO_RD1_Set()               (LATDSET = (1<<1))
#define GPIO_RD1_Clear()             (LATDCLR = (1<<1))
#define GPIO_RD1_Toggle()            (LATDINV= (1<<1))
#define GPIO_RD1_Get()               ((PORTD >> 1) & 0x1)
#define GPIO_RD1_OutputEnable()      (TRISDCLR = (1<<1))
#define GPIO_RD1_InputEnable()       (TRISDSET = (1<<1))
#define GPIO_RD1_PIN                  GPIO_PIN_RD1
/*** Macros for GPIO_RD2 pin ***/
#define GPIO_RD2_Set()               (LATDSET = (1<<2))
#define GPIO_RD2_Clear()             (LATDCLR = (1<<2))
#define GPIO_RD2_Toggle()            (LATDINV= (1<<2))
#define GPIO_RD2_Get()               ((PORTD >> 2) & 0x1)
#define GPIO_RD2_OutputEnable()      (TRISDCLR = (1<<2))
#define GPIO_RD2_InputEnable()       (TRISDSET = (1<<2))
#define GPIO_RD2_PIN                  GPIO_PIN_RD2
/*** Macros for GPIO_RD3 pin ***/
#define GPIO_RD3_Set()               (LATDSET = (1<<3))
#define GPIO_RD3_Clear()             (LATDCLR = (1<<3))
#define GPIO_RD3_Toggle()            (LATDINV= (1<<3))
#define GPIO_RD3_Get()               ((PORTD >> 3) & 0x1)
#define GPIO_RD3_OutputEnable()      (TRISDCLR = (1<<3))
#define GPIO_RD3_InputEnable()       (TRISDSET = (1<<3))
#define GPIO_RD3_PIN                  GPIO_PIN_RD3
/*** Macros for GPIO_RD4 pin ***/
#define GPIO_RD4_Set()               (LATDSET = (1<<4))
#define GPIO_RD4_Clear()             (LATDCLR = (1<<4))
#define GPIO_RD4_Toggle()            (LATDINV= (1<<4))
#define GPIO_RD4_Get()               ((PORTD >> 4) & 0x1)
#define GPIO_RD4_OutputEnable()      (TRISDCLR = (1<<4))
#define GPIO_RD4_InputEnable()       (TRISDSET = (1<<4))
#define GPIO_RD4_PIN                  GPIO_PIN_RD4
/*** Macros for GPIO_RD5 pin ***/
#define GPIO_RD5_Set()               (LATDSET = (1<<5))
#define GPIO_RD5_Clear()             (LATDCLR = (1<<5))
#define GPIO_RD5_Toggle()            (LATDINV= (1<<5))
#define GPIO_RD5_Get()               ((PORTD >> 5) & 0x1)
#define GPIO_RD5_OutputEnable()      (TRISDCLR = (1<<5))
#define GPIO_RD5_InputEnable()       (TRISDSET = (1<<5))
#define GPIO_RD5_PIN                  GPIO_PIN_RD5
/*** Macros for GPIO_RD6 pin ***/
#define GPIO_RD6_Set()               (LATDSET = (1<<6))
#define GPIO_RD6_Clear()             (LATDCLR = (1<<6))
#define GPIO_RD6_Toggle()            (LATDINV= (1<<6))
#define GPIO_RD6_Get()               ((PORTD >> 6) & 0x1)
#define GPIO_RD6_OutputEnable()      (TRISDCLR = (1<<6))
#define GPIO_RD6_InputEnable()       (TRISDSET = (1<<6))
#define GPIO_RD6_PIN                  GPIO_PIN_RD6
/*** Macros for GPIO_RD7 pin ***/
#define GPIO_RD7_Set()               (LATDSET = (1<<7))
#define GPIO_RD7_Clear()             (LATDCLR = (1<<7))
#define GPIO_RD7_Toggle()            (LATDINV= (1<<7))
#define GPIO_RD7_Get()               ((PORTD >> 7) & 0x1)
#define GPIO_RD7_OutputEnable()      (TRISDCLR = (1<<7))
#define GPIO_RD7_InputEnable()       (TRISDSET = (1<<7))
#define GPIO_RD7_PIN                  GPIO_PIN_RD7


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_B = 0,
    GPIO_PORT_C = 1,
    GPIO_PORT_D = 2,
    GPIO_PORT_E = 3,
    GPIO_PORT_F = 4,
    GPIO_PORT_G = 5,
} GPIO_PORT;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RB0 = 0,
    GPIO_PIN_RB1 = 1,
    GPIO_PIN_RB2 = 2,
    GPIO_PIN_RB3 = 3,
    GPIO_PIN_RB4 = 4,
    GPIO_PIN_RB5 = 5,
    GPIO_PIN_RB6 = 6,
    GPIO_PIN_RB7 = 7,
    GPIO_PIN_RB8 = 8,
    GPIO_PIN_RB9 = 9,
    GPIO_PIN_RB10 = 10,
    GPIO_PIN_RB11 = 11,
    GPIO_PIN_RB12 = 12,
    GPIO_PIN_RB13 = 13,
    GPIO_PIN_RB14 = 14,
    GPIO_PIN_RB15 = 15,
    GPIO_PIN_RC12 = 28,
    GPIO_PIN_RC13 = 29,
    GPIO_PIN_RC14 = 30,
    GPIO_PIN_RC15 = 31,
    GPIO_PIN_RD0 = 32,
    GPIO_PIN_RD1 = 33,
    GPIO_PIN_RD2 = 34,
    GPIO_PIN_RD3 = 35,
    GPIO_PIN_RD4 = 36,
    GPIO_PIN_RD5 = 37,
    GPIO_PIN_RD6 = 38,
    GPIO_PIN_RD7 = 39,
    GPIO_PIN_RD8 = 40,
    GPIO_PIN_RD9 = 41,
    GPIO_PIN_RD10 = 42,
    GPIO_PIN_RD11 = 43,
    GPIO_PIN_RE0 = 48,
    GPIO_PIN_RE1 = 49,
    GPIO_PIN_RE2 = 50,
    GPIO_PIN_RE3 = 51,
    GPIO_PIN_RE4 = 52,
    GPIO_PIN_RE5 = 53,
    GPIO_PIN_RE6 = 54,
    GPIO_PIN_RE7 = 55,
    GPIO_PIN_RF0 = 64,
    GPIO_PIN_RF1 = 65,
    GPIO_PIN_RF2 = 66,
    GPIO_PIN_RF3 = 67,
    GPIO_PIN_RF4 = 68,
    GPIO_PIN_RF5 = 69,
    GPIO_PIN_RF6 = 70,
    GPIO_PIN_RG2 = 82,
    GPIO_PIN_RG3 = 83,
    GPIO_PIN_RG6 = 86,
    GPIO_PIN_RG7 = 87,
    GPIO_PIN_RG8 = 88,
    GPIO_PIN_RG9 = 89,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;


void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite(pin>>4, (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead(pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead(pin>>4) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable(pin>>4, 0x1 << (pin & 0xF));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
